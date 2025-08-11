#include "HealthSubsystem.h"
#include "HealthComponent.h"
#include "AZombieCharacter.h"
#include "LightHouseGameModeBase.h"
#include "LighthouseGameState.h"
#include "CHCharacter.h"
#include "Engine/World.h"

void UHealthSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UHealthSubsystem::InitializeHealthForActor(AActor* Actor)
{
    if (!Actor) return;

    if (UHealthComponent* HC = Actor->FindComponentByClass<UHealthComponent>())
    {
        // 플레이어 (네 실제 클래스명에 맞게 유지: ACHCharacter)
        if (Actor->IsA(ACHCharacter::StaticClass()))
        {
            HC->Initialize(DefaultPlayerHP, ETeam::Player);
            HC->bDestroyOwnerOnDeath = false;

            // ★ Dynamic delegate 바인딩
            HC->OnDied.AddDynamic(this, &UHealthSubsystem::HandlePlayerDied);
        }
        // 좀비
        else if (Actor->IsA(AAZombieCharacter::StaticClass()))
        {
            HC->Initialize(DefaultZombieHP, ETeam::Zombie);
            HC->bDestroyOwnerOnDeath = bDestroyZombieOnDeath;

            // ★ Dynamic delegate 바인딩
            HC->OnDied.AddDynamic(this, &UHealthSubsystem::HandleZombieDied);
        }
        else
        {
            HC->Initialize(HC->GetMaxHealth(), ETeam::Neutral);
        }
    }
}

// ===== 바인딩 대상 구현 =====

void UHealthSubsystem::HandlePlayerDied(AActor* Dead)
{
    if (!Dead) return;

    if (UWorld* World = Dead->GetWorld())
    {
        if (ALightHouseGameModeBase* GM = World->GetAuthGameMode<ALightHouseGameModeBase>())
        {
            GM->StopRun();  // 플레이어 사망 시 러닝 중지 (아래 2번에서 GameMode에 구현)
        }
    }
}

void UHealthSubsystem::HandleZombieDied(AActor* Dead)
{
    if (!Dead) return;

    if (UWorld* World = Dead->GetWorld())
    {
        if (ALighthouseGameState* GS = World->GetGameState<ALighthouseGameState>())
        {
            // 좀비 카운트 쓰면 주석 해제
            // GS->AddRemainingZombies(-1);
        }
    }
}
