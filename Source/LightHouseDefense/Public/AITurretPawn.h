// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TurretState.h" // 위에 정의한 Enum 헤더 파일을 포함합니다.
#include "HealthComponent.h"
#include "HealthSubsystem.h"
#include "AITurretPawn.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UBehaviorTree;

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

    // 터렛의 각 컴포넌트들
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* TurretBody;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* TurretNeckPivot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* TurretNeck;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* TurretHead;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* MuzzleLocation;

    // 비헤이비어 트리가 사용할 변수들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float RotationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float FireRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float FireRange = 2000.f;



    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    // 현재 터렛 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    ETurretState CurrentState;

    // 터렛 상태를 설정하는 함수 (BT에서 사용)
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetTurretState(ETurretState NewState);

    // AI가 호출할 행동 함수들
    UFUNCTION(BlueprintCallable, Category = "AI")
    void DisableTurret();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void EnableTurret();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void Fire();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void InstallTurret();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void ScanForTargets(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void EnableTurretNeckbowdown();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void BEnableTurretNeckbowdown();
    // 발사 타이머 핸들
    FTimerHandle FireTimerHandle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float TurretDamage = 20.0f; // 기본 데미지 양

protected:
    // 스캔 회전 로직에 사용되는 변수
    float CurrentYawRotation = 0.0f;

};
