#include "LightHouseGameModeBase.h"
#include "WaveManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "LightHouseGameInstance.h"
#include "LighthouseHUD.h"
#include "LighthouseGameState.h"
#include "ZombieSpawner.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "AZombieCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/SkeletalMeshComponent.h"

ALightHouseGameModeBase::ALightHouseGameModeBase()
{
    GameStateClass = ALighthouseGameState::StaticClass();
    HUDClass = ALighthouseHUD::StaticClass();
}

void ALightHouseGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    bRunStopped = false;

    if (ULightHouseGameInstance* GI = Cast<ULightHouseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode에서 GameInstance 확인 완료"));
    }
}

// === 새로 추가 ===
void ALightHouseGameModeBase::StartRun()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // 1) 타이머 시작
    if (ALighthouseGameState* GS = World->GetGameState<ALighthouseGameState>())
    {
        GS->StartCountdown(); // FIX: F키로 시작 시 여기서만 카운트다운 시작
    }

    // 2) (선택) 웨이브 진행 타이머 사용 중이면 여기서 Resume/Start
    //if (auto* WM = GetGameInstance()->GetSubsystem<UWaveManagerSubsystem>())
    //{
    //    WM->StartWaveProgression(); // 사용 중이 아니면 주석 처리 OK
    //}

    // 3) 모든 스포너 시작
    for (TActorIterator<AZombieSpawner> It(World); It; ++It)
    {
        It->StartSpawning(); // FIX: 수동 시작
    }

    UGameplayStatics::SetGamePaused(World, false);
    UE_LOG(LogTemp, Warning, TEXT("[GM] StartRun -> Game unpaused, Spawns/Timer started"));
}

void ALightHouseGameModeBase::StopRun()
{
    UWorld* World = GetWorld();
    if (!World) return;

    if (bRunStopped) return;
    bRunStopped = true;

    if (auto* WM = GetGameInstance()->GetSubsystem<UWaveManagerSubsystem>())
        WM->PauseWaveTimer();

    if (auto* GS = World->GetGameState<ALighthouseGameState>())
        GS->PauseCountdown();

    for (TActorIterator<AZombieSpawner> It(World); It; ++It)
        It->StopSpawning();

    // (보강) 이미 스폰된 좀비들 정지(기존 로직 유지)
    for (TActorIterator<AAZombieCharacter> ZIt(World); ZIt; ++ZIt)
    {
        AAZombieCharacter* Z = *ZIt;

        if (UCharacterMovementComponent* Move = Z->GetCharacterMovement())
        {
            Move->StopMovementImmediately();
            Move->DisableMovement();
        }

        if (AAIController* AI = Cast<AAIController>(Z->GetController()))
        {
            AI->StopMovement();

            UBrainComponent* Brain = nullptr;
#if ENGINE_MAJOR_VERSION >= 5
            Brain = AI->GetBrainComponent();
#endif
            if (!Brain) { Brain = AI->FindComponentByClass<UBrainComponent>(); }
            if (Brain) { Brain->StopLogic(TEXT("GameOver")); }
        }

        if (USkeletalMeshComponent* Mesh = Z->GetMesh())
        {
            Mesh->bPauseAnims = true;
        }

        Z->SetActorTickEnabled(false);
    }

    UGameplayStatics::SetGamePaused(World, true);
    UE_LOG(LogTemp, Warning, TEXT("[GM] StopRun -> GamePaused, Spawns/Timers stopped"));
}
