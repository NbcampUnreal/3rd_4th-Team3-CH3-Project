#include "ZombieSpawner.h"
#include "Kismet/GameplayStatics.h"
//#include "WaveManagerSubsystem.h"
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
    //SpawnZombies();
    
     // ======================== 변경 ========================
    // 이제부터는 고정 인터벌 2개의 타이머를 '무한 반복'으로 돌린다.
    if (UWorld* World = GetWorld())
    {
        if (NormalZombieClass && NormalSpawnInterval > 0.f)
        {
            World->GetTimerManager().SetTimer(
                NormalTimerHandle,
                this,
                &AZombieSpawner::SpawnSingleNormal,
                NormalSpawnInterval,
                true // 반복
            );
        }

        if (TankZombieClass && TankSpawnInterval > 0.f)
        {
            World->GetTimerManager().SetTimer(
                TankTimerHandle,
                this,
                &AZombieSpawner::SpawnSingleTank,
                TankSpawnInterval,
                true // 반복
            );
        }

        UE_LOG(LogTemp, Log, TEXT("[Spawner] StartSpawning(): Normal=%.2fs, Tank=%.2fs"),
            NormalSpawnInterval, TankSpawnInterval);
    }

 
}

void AZombieSpawner::StopSpawning()
{
   // GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
   //SpawnedCount = 0;

    // ====================== 변경 ========================
    // 게임 오버/정지 등에서 타이머를 모두 제거 (무한 반복 종료)
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(NormalTimerHandle);
        World->GetTimerManager().ClearTimer(TankTimerHandle);
    }
}

void AZombieSpawner::PauseSpawning()
{
   // GetWorldTimerManager().PauseTimer(SpawnTimerHandle);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().PauseTimer(NormalTimerHandle);
        World->GetTimerManager().PauseTimer(TankTimerHandle);
    }
}

void AZombieSpawner::ResumeSpawning()
{
   // GetWorldTimerManager().UnPauseTimer(SpawnTimerHandle);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().UnPauseTimer(NormalTimerHandle);
        World->GetTimerManager().UnPauseTimer(TankTimerHandle);
    }
}


// ======================== [ADDED] ========================
AActor* AZombieSpawner::PickRandomSpawnPoint() const
{
    if (SpawnPoints.Num() == 0) return nullptr;
    const int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
    return SpawnPoints[Index];
}

void AZombieSpawner::SpawnSingleNormal()
{
    if (!NormalZombieClass) return;

    AActor* SpawnAt = PickRandomSpawnPoint();
    if (!SpawnAt) return;

    FActorSpawnParameters Params;
    if (AAZombieCharacter* NewZombie = GetWorld()->SpawnActor<AAZombieCharacter>(
        NormalZombieClass,
        SpawnAt->GetActorLocation(),
        SpawnAt->GetActorRotation(),
        Params))
    {
        // 스폰 직후 HS 초기화(Alive++/OnDied 바인딩)
        if (UHealthSubsystem* HS = GetGameInstance()->GetSubsystem<UHealthSubsystem>())
        {
            HS->InitializeHealthForActor(NewZombie);
        }

        // (디버그) 주기적으로 스폰되는지 확인
        // UKismetSystemLibrary::PrintString(this, TEXT("[Spawner] Spawned NORMAL"));
    }
}

void AZombieSpawner::SpawnSingleTank()
{
    if (!TankZombieClass) return;

    AActor* SpawnAt = PickRandomSpawnPoint();
    if (!SpawnAt) return;

    FActorSpawnParameters Params;
    if (AAZombieCharacter* NewZombie = GetWorld()->SpawnActor<AAZombieCharacter>(
        TankZombieClass,
        SpawnAt->GetActorLocation(),
        SpawnAt->GetActorRotation(),
        Params))
    {
        // 스폰 직후 HS 초기화(Alive++/OnDied 바인딩)
        if (UHealthSubsystem* HS = GetGameInstance()->GetSubsystem<UHealthSubsystem>())
        {
            HS->InitializeHealthForActor(NewZombie);
        }

        // (디버그) 120초마다 스폰되는지 확인
        // UKismetSystemLibrary::PrintString(this, TEXT("[Spawner] Spawned TANK"));
    }
}

//void AZombieSpawner::SpawnZombies()
//{
//    if (!ZombieClass) return;
//
//    if (UWaveManagerSubsystem* WaveManager = GetGameInstance()->GetSubsystem<UWaveManagerSubsystem>())
//    {
//        TargetSpawnCount = WaveManager->GetCurrentZombieCount();
//        CurrentSpawnInterval = WaveManager->GetCurrentSpawnInterval();
//        SpawnedCount = 0;
//
//        GetWorldTimerManager().SetTimer(
//            SpawnTimerHandle,
//            this,
//            &AZombieSpawner::SpawnSingleZombie,
//            CurrentSpawnInterval,
//            true
//        );
//    }
//   
//}

//void AZombieSpawner::SpawnSingleZombie()
//{
//    if (SpawnedCount >= TargetSpawnCount)
//    {
//        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
//        return;
//    }
//
//    if (SpawnPoints.Num() == 0) return;
//
//    const int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
//    AActor* SpawnLocation = SpawnPoints[Index];
//
//    if (SpawnLocation)
//    {
//        FActorSpawnParameters SpawnParams;
//
//        if (AAZombieCharacter* NewZombie = GetWorld()->SpawnActor<AAZombieCharacter>(
//            ZombieClass,
//            SpawnLocation->GetActorLocation(),
//            SpawnLocation->GetActorRotation(),
//            SpawnParams))
//        {
//            // FIX: 스폰 직후 HS 초기화(Alive++/OnDied 바인딩)
//            if (UHealthSubsystem* HS = GetGameInstance()->GetSubsystem<UHealthSubsystem>())
//            {
//                HS->InitializeHealthForActor(NewZombie);
//            }
//
//            ++SpawnedCount;
//        }
//    }
//}



