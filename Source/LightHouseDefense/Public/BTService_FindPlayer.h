// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_FindPlayer.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API UBTService_FindPlayer : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:
    UBTService_FindPlayer();

protected:
    //TickNode함수를 오버라이드하여 주기적으로 플레이어를 감지
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    //블랙보드에 저장할 타겟 액터 키
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    FBlackboardKeySelector TargetActorKey;

};
