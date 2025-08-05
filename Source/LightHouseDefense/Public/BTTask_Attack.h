#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h" // FBlackboardKeySelector 사용을 위해 필요
#include "BTTask_Attack.generated.h" 


UCLASS(Blueprintable)
class LIGHTHOUSEDEFENSE_API UBTTask_Attack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    //생성자
    UBTTask_Attack();
protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard")
    FBlackboardKeySelector TargetActorKey;
};

