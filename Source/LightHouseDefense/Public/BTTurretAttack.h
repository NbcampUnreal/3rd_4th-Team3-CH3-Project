// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTurretAttack.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTHOUSEDEFENSE_API UBTTurretAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
    UBTTurretAttack();
protected:

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowPrivateAccess = "true"))
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float AttackInterval ; // 공격 주기 (예: 0.5초마다 공격)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float AttackDamage; // 공격 데미지

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float LineTraceRange ; // 라인 트레이스 최대 거리 (예: 2000.0f = 20미터)

protected:
    // 마지막 공격 시간을 저장할 변수 
    float LastAttackTime;

    // AI가 컨트롤하는 터렛의 총구 위치를 위한 컴포넌트 이름 (옵션)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FName MuzzleComponentName = TEXT("Muzzle"); // 캐릭터에 총구 StaticMeshCompon

};
