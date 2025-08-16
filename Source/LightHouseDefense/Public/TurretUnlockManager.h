#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretUnlockManager.generated.h"

class AAITurretPawn;
class UHealthComponent;
class AAZombieCharacter;      
class ATankZombieCharacter;   

UCLASS()
class LIGHTHOUSEDEFENSE_API ATurretUnlockManager : public AActor
{
    GENERATED_BODY()

public:
    ATurretUnlockManager();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // [추가] 월드 스폰 감시 핸들
    FDelegateHandle SpawnHandle;

    // [추가] 스폰된 액터가 좀비면 HealthComponent의 OnDied에 바인딩
    void OnActorSpawned(AActor* SpawnedActor);
    void TryBindZombie(AActor* Actor);

    UFUNCTION()
    void HandleZombieDied(AActor* DeadActor);

    void CheckUnlocks();

    // [추가] 이미 바인딩된 좀비 중복 방지
    UPROPERTY()
    TSet<TWeakObjectPtr<AActor>> BoundZombies;

    // [추가] 현재 카운트
    UPROPERTY(VisibleAnywhere, Category = "Stats")
    int32 NormalKills = 0;

    UPROPERTY(VisibleAnywhere, Category = "Stats")
    int32 TankKills = 0;

    // [추가] 다음에 열 차례의 터렛 인덱스
    int32 NextUnlockIndex = 0;

public:
    // [세팅] 레벨에 배치된 4대 터렛을 할당
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Turret")
    TArray<AAITurretPawn*> Turrets;

    // [세팅] 각 단계별 요구치(동일 인덱스끼리 AND 조건)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds")
    TArray<int32> NormalKillThresholds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds")
    TArray<int32> TankKillThresholds;
};
