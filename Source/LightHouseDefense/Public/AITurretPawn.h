// AITurretPawn.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "AITurretPawn.generated.h"


class UBehaviorTree;

// 터렛의 현재 상태를 관리하는 Enum
UENUM(BlueprintType)
enum class ETurretState : uint8
{
    Scanning,
    Attacking,
    Disabled,
    Idle // 설치되었지만 비활성 상태
};

UCLASS()
class LIGHTHOUSEDEFENSE_API AAITurretPawn : public APawn
{
    GENERATED_BODY()

public:
    AAITurretPawn();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // 터렛의 시각적 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    class UStaticMeshComponent* TurretBody;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    class USceneComponent* TurretNeckPivot; // 회전을 위한 피벗 포인트

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    class UStaticMeshComponent* TurretNeck;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    class UStaticMeshComponent* TurretHead;

    //총구
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class USceneComponent* MuzzleLocation;

    // 터렛의 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    ETurretState CurrentState;

    // 터렛의 회전 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float RotationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRate;

    FTimerHandle FireTimerHandle;

    UFUNCTION(BlueprintCallable, Category = "Turret")
    void EnableTurretNeckbowdown();

    UFUNCTION(BlueprintCallable, Category = "Turret")
    void DisableTurret();

    UFUNCTION(BlueprintCallable, Category = "Turret")
    void EnableTurret();

    UFUNCTION()
    void Fire();

    UFUNCTION(BlueprintCallable, Category = "Turret")
    void InstallTurret();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
    float FireRange = 100000.0f;
private:
    float CurrentYawRotation;
    void ScanForTargets(float DeltaTime);

};
