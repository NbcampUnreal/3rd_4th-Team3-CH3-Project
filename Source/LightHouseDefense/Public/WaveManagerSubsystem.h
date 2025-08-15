#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WaveManagerSubsystem.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API UWaveManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 난이도 진행 시작
    void StartWaveProgression();

    // 난이도 상승 처리
    void IncreaseDifficulty();

    // 외부에서 현재 값 접근
    int32 GetCurrentZombieCount() const;
    float GetCurrentSpawnInterval() const;
    int32 GetCurrentDifficultyLevel() const;

    // 게임 일시정지, 재개 (예: 상점 입장 시)
    void PauseWaveTimer();
    void ResumeWaveTimer();

private:
    FTimerHandle DifficultyTimerHandle;

    float DifficultyIncreaseInterval = 1.0f;   // 난이도 상승 주기
    int32 ZombieSpawnCount = 9999;                // 초기 좀비 수
    float ZombieSpawnInterval = 3.0f;          // 초기 스폰 간격

    int32 DifficultyLevel = 0;                 // 현재 난이도
};
