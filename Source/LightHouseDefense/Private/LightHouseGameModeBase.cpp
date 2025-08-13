#include "LightHouseGameModeBase.h"
#include "WaveManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "LightHouseGameInstance.h"
#include "LighthouseHUD.h"
#include "LighthouseGameState.h"
#include "ZombieSpawner.h"
#include "EngineUtils.h"  
#include "Engine/World.h" // 추가된 헤더
#include "AZombieCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h" // 좀비 AI 정지를 위해 컨트롤러/브레인 컴포넌트 필요
#include "BrainComponent.h"
#include "Components/SkeletalMeshComponent.h" // 이미 스폰된 좀비의 애니메이션까지 멈추고 싶다면

ALightHouseGameModeBase::ALightHouseGameModeBase()
{
    // GameMode에서 사용할 GameState/HUD 지정
    GameStateClass = ALighthouseGameState::StaticClass();
    HUDClass = ALighthouseHUD::StaticClass();
}

void ALightHouseGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // StopRun 가드 플래그 매 게임 시작 시 리셋
    bRunStopped = false;

    // GameInstance를 통해 난이도 시스템 시작 로그 확인
    if (ULightHouseGameInstance* GI = Cast<ULightHouseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode에서 GameInstance 확인 완료"));
    }
}

void ALightHouseGameModeBase::StopRun()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // 사망 등으로 StopRun이 중복 호출되어도 1번만 실행되도록 보호
    if (bRunStopped) return;
    bRunStopped = true;

    // 웨이브 난이도/카운트다운/스포너 멈춤 (네 프로젝트 로직에 맞춰 조정)
    if (auto* WM = GetGameInstance()->GetSubsystem<UWaveManagerSubsystem>())
        WM->PauseWaveTimer();

    if (auto* GS = World->GetGameState<ALighthouseGameState>())
        GS->PauseCountdown();

    for (TActorIterator<AZombieSpawner> It(World); It; ++It)
        It->StopSpawning();

   
    // (보강) 이미 스폰돼 있던 좀비도 완전히 멈추기 — 단 한 번만 수행
    for (TActorIterator<AAZombieCharacter> ZIt(World); ZIt; ++ZIt)
    {
        AAZombieCharacter* Z = *ZIt;

        // 1) 이동 정지
        if (UCharacterMovementComponent* Move = Z->GetCharacterMovement())
        {
            Move->StopMovementImmediately();
            Move->DisableMovement();
        }

        // 2) AI 로직/이동 정지 (컨트롤러가 AIController일 때)
        if (AAIController* AI = Cast<AAIController>(Z->GetController()))
        {
            AI->StopMovement(); // PathFollowing 끊기

            // BrainComponent 안전 접근 (보호 수준/엔진 버전 대응)
            UBrainComponent* Brain = nullptr;
#if ENGINE_MAJOR_VERSION >= 5
            Brain = AI->GetBrainComponent();     // UE5에서 공개 Getter
#endif
            if (!Brain)
            {
                Brain = AI->FindComponentByClass<UBrainComponent>(); // 백업 경로
            }
            if (Brain)
            {
                Brain->StopLogic(TEXT("GameOver"));  // BT/서비스/태스크 정지
            }
        }

        // 3) (선택) 애니메이션도 멈추고 싶다면
        if (USkeletalMeshComponent* Mesh = Z->GetMesh())
        {
            Mesh->bPauseAnims = true;                // 필요 없으면 이 줄은 주석 처리
        }

        // 4) (선택) 액터 자체 Tick 불가
        Z->SetActorTickEnabled(false);               // 필요 없으면 주석 처리
    }
    

    // 필요 시 게임 일시정지, 실패 UI 호출 등 추가
    UGameplayStatics::SetGamePaused(World, true);

    // 디버그 로그
    UE_LOG(LogTemp, Warning, TEXT("[GM] StopRun called → SetGamePaused(true)"));
    UE_LOG(LogTemp, Warning, TEXT("[GM] StopRun -> GamePaused, Spawns/Timers stopped"));
}
