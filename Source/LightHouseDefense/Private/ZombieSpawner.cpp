// === ZombieSpawner.cpp ===
#include "ZombieSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "WaveManagerSubsystem.h"
#include "AZombieCharacter.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"
#include "HealthSubsystem.h" // FIX

AZombieSpawner::AZombieSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AZombieSpawner::BeginPlay()
{
    Super::BeginPlay();

    // FIX: 자동 시작 비활성화(주석). GameMode::StartRun()에서 StartSpawning() 호출.
    // GetWorldTimerManager().SetTimerForNextTick(this, &AZombieSpawner::SpawnZombies);
}

void AZombieSpawner::StartSpawning()
{
    // FIX: 수동 시작 진입점 — 기존 SpawnZombies() 호출
    SpawnZombies();
}

void AZombieSpawner::SpawnZombies()
{
    if (!ZombieClass) return;

    if (UWaveManagerSubsystem* WaveManager = GetGameInstance()->GetSubsystem<UWaveManagerSubsystem>())
    {
        TargetSpawnCount = WaveManager->GetCurrentZombieCount();
        CurrentSpawnInterval = WaveManager->GetCurrentSpawnInterval();
        SpawnedCount = 0;

        GetWorldTimerManager().SetTimer(
            SpawnTimerHandle,
            this,
            &AZombieSpawner::SpawnSingleZombie,
            CurrentSpawnInterval,
            true
        );
    }
}

void AZombieSpawner::SpawnSingleZombie()
{
    if (SpawnedCount >= TargetSpawnCount)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        return;
    }

    if (SpawnPoints.Num() == 0) return;

    const int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
    AActor* SpawnLocation = SpawnPoints[Index];

    if (SpawnLocation)
    {
        FActorSpawnParameters SpawnParams;

        if (AAZombieCharacter* NewZombie = GetWorld()->SpawnActor<AAZombieCharacter>(
            ZombieClass,
            SpawnLocation->GetActorLocation(),
            SpawnLocation->GetActorRotation(),
            SpawnParams))
        {
            // FIX: 스폰 직후 HS 초기화(Alive++/OnDied 바인딩)
            if (UHealthSubsystem* HS = GetGameInstance()->GetSubsystem<UHealthSubsystem>())
            {
                HS->InitializeHealthForActor(NewZombie);
            }

            ++SpawnedCount;
        }
    }
}

void AZombieSpawner::StopSpawning()
{
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
    SpawnedCount = 0;
}

void AZombieSpawner::PauseSpawning()
{
    GetWorldTimerManager().PauseTimer(SpawnTimerHandle);
}

void AZombieSpawner::ResumeSpawning()
{
    GetWorldTimerManager().UnPauseTimer(SpawnTimerHandle);
}
