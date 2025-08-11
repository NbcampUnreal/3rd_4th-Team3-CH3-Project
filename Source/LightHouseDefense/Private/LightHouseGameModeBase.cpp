#include "LightHouseGameModeBase.h"
#include "WaveManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "LightHouseGameInstance.h"
#include "LighthouseHUD.h"
#include "LighthouseGameState.h"
#include "ZombieSpawner.h"
#include "EngineUtils.h"  
#include "Engine/World.h" // 추가된 헤더

ALightHouseGameModeBase::ALightHouseGameModeBase()
{
    GameStateClass = ALighthouseGameState::StaticClass();
    HUDClass = ALighthouseHUD::StaticClass();
}

void ALightHouseGameModeBase::BeginPlay()
{
    Super::BeginPlay();

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

    // 웨이브 난이도/카운트다운/스포너 멈춤 (네 프로젝트 로직에 맞춰 조정)
    if (auto* WM = GetGameInstance()->GetSubsystem<UWaveManagerSubsystem>())
        WM->PauseWaveTimer();

    if (auto* GS = World->GetGameState<ALighthouseGameState>())
        GS->PauseCountdown();

    for (TActorIterator<AZombieSpawner> It(World); It; ++It)
        It->StopSpawning();

    // 필요 시 게임 일시정지, 실패 UI 호출 등 추가
    // UGameplayStatics::SetGamePaused(World, true);
}
