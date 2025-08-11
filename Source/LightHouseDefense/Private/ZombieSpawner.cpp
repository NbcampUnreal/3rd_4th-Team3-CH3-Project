#include "ZombieSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "WaveManagerSubsystem.h"
#include "AZombieCharacter.h" // 기본 좀비 클래스
#include "Engine/World.h"
#include "Engine/GameInstance.h" 
#include "TimerManager.h"

AZombieSpawner::AZombieSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AZombieSpawner::BeginPlay()
{
    Super::BeginPlay();

    // 일정 간격마다 좀비를 소환하는 타이머 시작
    GetWorldTimerManager().SetTimerForNextTick(this, &AZombieSpawner::SpawnZombies);
}

void AZombieSpawner::SpawnZombies()
{
    if (!ZombieClass) return;

    // 현재 난이도 기반 정보 가져오기
    if (UWaveManagerSubsystem* WaveManager = GetGameInstance()->GetSubsystem<UWaveManagerSubsystem>())
    {
        TargetSpawnCount = WaveManager->GetCurrentZombieCount();
        CurrentSpawnInterval = WaveManager->GetCurrentSpawnInterval();
        SpawnedCount = 0;


        // 일정 간격마다 SpawnSingleZombie 실행
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
        // 목표 수만큼 스폰했으면 종료
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        return;
    }

    if (SpawnPoints.Num() == 0) return;

    // 랜덤한 스폰 지점 선택
    int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
    AActor* SpawnLocation = SpawnPoints[Index];

    if (SpawnLocation)
    {
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AAZombieCharacter>(
            ZombieClass,
            SpawnLocation->GetActorLocation(),
            SpawnLocation->GetActorRotation(),
            SpawnParams
        );

        SpawnedCount++;
    }

}
