#include "TurretUnlockManager.h"
#include "AITurretPawn.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HealthComponent.h"
#include "AZombieCharacter.h"      
#include "TankZombieCharacter.h" 


// [추가] 기본값 세팅
ATurretUnlockManager::ATurretUnlockManager()
{
    PrimaryActorTick.bCanEverTick = false;

    // 요청하신 기본 임계값
    NormalKillThresholds = { 20, 40, 60, 80 };
    TankKillThresholds = { 2,  4,  6,  8 };
}

void ATurretUnlockManager::BeginPlay()
{
    Super::BeginPlay();

    // [추가] 시작 시 모든 터렛을 "확실히" 비활성화
    for (AAITurretPawn* T : Turrets)
    {
        if (T) { T->DisableTurret(); }
    }

    // [추가] 이미 월드에 존재하는 좀비들 바인딩
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        TryBindZombie(*It);
    }

    // [추가] 앞으로 스폰되는 좀비도 바인딩
    SpawnHandle = GetWorld()->AddOnActorSpawnedHandler(
        FOnActorSpawned::FDelegate::CreateUObject(this, &ATurretUnlockManager::OnActorSpawned)
    );
}

void ATurretUnlockManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SpawnHandle.IsValid() && GetWorld())
    {
        GetWorld()->RemoveOnActorSpawnedHandler(SpawnHandle);
    }
    Super::EndPlay(EndPlayReason);
}

void ATurretUnlockManager::OnActorSpawned(AActor* SpawnedActor)
{
    TryBindZombie(SpawnedActor);
}

void ATurretUnlockManager::TryBindZombie(AActor* Actor)
{
    if (!Actor) return;

    // [추가] HealthComponent가 있고, 팀이 Zombie라면 바인딩
    if (UHealthComponent* HC = Actor->FindComponentByClass<UHealthComponent>())
    {
        if (HC->Team == ETeam::Zombie)
        {
            // 중복 바인딩 방지
            if (!BoundZombies.Contains(Actor))
            {
                HC->OnDied.AddDynamic(this, &ATurretUnlockManager::HandleZombieDied);
                BoundZombies.Add(Actor);
            }
        }
    }
}

void ATurretUnlockManager::HandleZombieDied(AActor* DeadActor)
{
    if (!DeadActor) return;

    // [추가] 팀이 좀비면 카운트
    if (UHealthComponent* HC = DeadActor->FindComponentByClass<UHealthComponent>())
    {
        if (HC->Team == ETeam::Zombie)
        {
            // 탱크 여부는 타입으로 구분(탱크 파생이면 탱크)
            if (DeadActor->IsA(AAZombieCharacter::StaticClass()) && !DeadActor->IsA(ATankZombieCharacter::StaticClass()))
            {
                ++NormalKills;
            }
            else if (DeadActor->IsA(ATankZombieCharacter::StaticClass()))
            {
                ++TankKills;
            }

            CheckUnlocks();
        }
    }
}

void ATurretUnlockManager::CheckUnlocks()
{
    // [추가] 배열 길이 보호
    const int32 MaxStages = FMath::Min3(
        4, // 논리상 4대
        NormalKillThresholds.Num(),
        TankKillThresholds.Num()
    );

    while (NextUnlockIndex < MaxStages)
    {
        const int32 NeedNormal = NormalKillThresholds[NextUnlockIndex];
        const int32 NeedTank = TankKillThresholds[NextUnlockIndex];

        if (NormalKills >= NeedNormal && TankKills >= NeedTank)
        {
            // 해당 인덱스 터렛 활성화
            if (Turrets.IsValidIndex(NextUnlockIndex))
            {
                if (AAITurretPawn* T = Turrets[NextUnlockIndex])
                {
                    T->EnableTurret();
                    UE_LOG(LogTemp, Log, TEXT("[TurretUnlock] Stage %d -> Turret %s ENABLED (Normal:%d/%d, Tank:%d/%d)"),
                        NextUnlockIndex + 1, *T->GetName(), NormalKills, NeedNormal, TankKills, NeedTank);
                }
            }
            ++NextUnlockIndex; // 다음 단계로
        }
        else
        {
            break; // 아직 조건 미달이면 중단
        }
    }
}
