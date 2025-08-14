// === HealthSubsystem.cpp ===
#include "HealthSubsystem.h"
#include "HealthComponent.h"
#include "AZombieCharacter.h"
#include "LightHouseGameModeBase.h"
#include "LighthouseGameState.h"
#include "CHCharacter.h"
#include "LightHouseCharacter.h"  // FIX: 추가
#include "Engine/World.h"

// (기본 초기화)
void UHealthSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UHealthSubsystem::InitializeHealthForActor(AActor* Actor)
{
    if (!Actor) return;
    UHealthComponent* HC = Actor->FindComponentByClass<UHealthComponent>();
    if (!HC) return;

    // ───────────────── 등대 ─────────────────
    if (Actor->IsA(ALightHouseCharacter::StaticClass()))
    {
        HC->Initialize(500.f, ETeam::Player);
        HC->bDestroyOwnerOnDeath = false;

        // FIX: PIE 재시작 시 델리게이트 누적 방지
        if (!HC->OnDied.IsAlreadyBound(this, &UHealthSubsystem::HandlePlayerDied))
        {
            HC->OnDied.AddDynamic(this, &UHealthSubsystem::HandlePlayerDied);
        }
        return;
    }

    // ───────────────── 플레이어 ─────────────────
    if (Actor->IsA(ACHCharacter::StaticClass()))
    {
        HC->Initialize(100.f, ETeam::Player);
        HC->bDestroyOwnerOnDeath = false;

        // FIX: PIE 재시작 시 델리게이트 누적 방지
        if (!HC->OnDied.IsAlreadyBound(this, &UHealthSubsystem::HandlePlayerDied))
        {
            HC->OnDied.AddDynamic(this, &UHealthSubsystem::HandlePlayerDied);
        }
        return;
    }

    // ───────────────── 좀비 ─────────────────
    if (Actor->IsA(AAZombieCharacter::StaticClass()))
    {
        HC->Initialize(DefaultZombieHP, ETeam::Zombie);
        HC->bDestroyOwnerOnDeath = bDestroyZombieOnDeath;

        // Alive++ 로직
        if (UWorld* World = Actor->GetWorld())
        {
            if (ALighthouseGameState* GS = World->GetGameState<ALighthouseGameState>())
            {
                GS->IncAliveZombiesTotal();
            }
        }

        // FIX: 사망 시 Alive-- / Kill++ 연동 (중복 바인딩 방지)
        if (!HC->OnDied.IsAlreadyBound(this, &UHealthSubsystem::HandleZombieDied))
        {
            HC->OnDied.AddDynamic(this, &UHealthSubsystem::HandleZombieDied);
        }
        return;
    }

    // ───────────────── 기타(중립) ─────────────────
    HC->Initialize(HC->GetMaxHealth(), ETeam::Neutral);
}

// ===== 바인딩 대상 구현 =====

void UHealthSubsystem::HandlePlayerDied(AActor* Dead)
{
    if (!Dead) return;

    if (UWorld* World = Dead->GetWorld())
    {
        if (ALightHouseGameModeBase* GM = World->GetAuthGameMode<ALightHouseGameModeBase>())
        {
            // FIX: 플레이어 사망 시 게임 정지 등 처리
           GM->StopRun();
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
            // FIX: 좀비 사망 시 Alive-- & (원하면) 킬 수 증가
            GS->DecAliveZombiesTotal();

            // 필요하면 좀비 타입 구분해서 킬 카운트도 올려줘:
            // GS->AddKillNormal();
            // 탱크 분기까지 원하면 Dead->IsA(ATankZombieCharacter::StaticClass()) 체크해서 GS->AddKillTank();
        }
    }
}
