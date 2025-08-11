#include "WaveManagerSubsystem.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UWaveManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    // Subsystem이 생성될 때 호출됨. 필요한 초기화 작업은 여기에서 수행 가능.
    Super::Initialize(Collection);
}

void UWaveManagerSubsystem::StartWaveProgression()
{
    // 유효한 월드 객체 가져오기
    if (UWorld* World = GetWorld())
    {
        // 일정 시간 간격으로 IncreaseDifficulty() 호출하는 반복 타이머 설정
        World->GetTimerManager().SetTimer(
            DifficultyTimerHandle, // 타이머 핸들
            this, // 콜백 대상 객체
            &UWaveManagerSubsystem::IncreaseDifficulty,// 호출할 함수
            DifficultyIncreaseInterval, // 간격 (초)
            true // 반복 타이머
        );
    }
}

void UWaveManagerSubsystem::IncreaseDifficulty()
{
    DifficultyLevel++; // 난이도 레벨 증가


    ZombieSpawnCount += 2; // 소환되는 좀비 수 증가

    // 스폰 간격 감소 (최소 0.5초까지 감소)
    ZombieSpawnInterval = FMath::Max(ZombieSpawnInterval - 0.2f, 0.5f);

    // 디버그용 로그 출력
    UE_LOG(LogTemp, Log, TEXT("난이도 ↑: Lv%d | 좀비 수: %d | 스폰 간격: %.2f"),
        DifficultyLevel, ZombieSpawnCount, ZombieSpawnInterval);
}

// 현재 좀비 수 반환
int32 UWaveManagerSubsystem::GetCurrentZombieCount() const
{
    return ZombieSpawnCount;
}

// 현재 좀비 스폰 간격 반환
float UWaveManagerSubsystem::GetCurrentSpawnInterval() const
{
    return ZombieSpawnInterval;
}

// 현재 난이도 레벨 반환
int32 UWaveManagerSubsystem::GetCurrentDifficultyLevel() const
{
    return DifficultyLevel;
}

// 난이도 증가 타이머 일시정지 (게임 일시정지 시 활용 가능)
void UWaveManagerSubsystem::PauseWaveTimer()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().PauseTimer(DifficultyTimerHandle);
    }
}

// 난이도 증가 타이머 재개 (일시정지 해제 시 사용)
void UWaveManagerSubsystem::ResumeWaveTimer()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().UnPauseTimer(DifficultyTimerHandle);
    }
}
