#include "WaveManagerSubsystem.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UWaveManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UWaveManagerSubsystem::StartWaveProgression()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            DifficultyTimerHandle,
            this,
            &UWaveManagerSubsystem::IncreaseDifficulty,
            DifficultyIncreaseInterval,
            true // 반복 타이머
        );
    }
}

void UWaveManagerSubsystem::IncreaseDifficulty()
{
    DifficultyLevel++;

    // 좀비 수 점진적 증가
    ZombieSpawnCount += 2;

    // 스폰 간격 점진적 감소 → 더 빠르게 소환됨 (최소 0.5초 유지)
    ZombieSpawnInterval = FMath::Max(ZombieSpawnInterval - 0.2f, 0.5f);

    UE_LOG(LogTemp, Log, TEXT("난이도 ↑: Lv%d | 좀비 수: %d | 스폰 간격: %.2f"),
        DifficultyLevel, ZombieSpawnCount, ZombieSpawnInterval);
}

int32 UWaveManagerSubsystem::GetCurrentZombieCount() const
{
    return ZombieSpawnCount;
}

float UWaveManagerSubsystem::GetCurrentSpawnInterval() const
{
    return ZombieSpawnInterval;
}

int32 UWaveManagerSubsystem::GetCurrentDifficultyLevel() const
{
    return DifficultyLevel;
}

void UWaveManagerSubsystem::PauseWaveTimer()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().PauseTimer(DifficultyTimerHandle);
    }
}

void UWaveManagerSubsystem::ResumeWaveTimer()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().UnPauseTimer(DifficultyTimerHandle);
    }
}
