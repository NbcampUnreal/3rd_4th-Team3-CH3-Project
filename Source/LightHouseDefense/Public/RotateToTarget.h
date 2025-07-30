// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "RotateToTarget.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTHOUSEDEFENSE_API URotateToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	

public:
	

	URotateToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector TargetActorKey; // 비헤이비어 트리 에디터에서 선택할 블랙보드 키
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
};
