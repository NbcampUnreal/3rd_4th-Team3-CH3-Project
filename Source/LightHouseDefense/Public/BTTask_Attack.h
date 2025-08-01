// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTTask_Attack.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
    UBTTask_Attack();

    protected:
        //Tast 실행 시 호출되는 함수
        virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

        //블랙보드에서 타겟 액터 키를 가져오는 함수
        UPROPERTY(EditAnywhere,BluePrintReadWrite, Category="AI")
        FBlackboardKeySelector TargetActorKey;

        //공격 애니메이션을 재생할 시간
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
        float AttackDuration;

        //좀비의 공격 데미지(BlueprintCallable로 설정하여 블루프린트에서 호출 가능)
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
        float AttackDamage;

        //공격 범위(플레이어와 이 거리 안에 있을 때 공격)
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
        float AttackRange;
};
