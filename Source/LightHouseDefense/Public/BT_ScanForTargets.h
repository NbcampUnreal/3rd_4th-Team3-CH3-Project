#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_ScanForTargets.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API UBT_ScanForTargets : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBT_ScanForTargets();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    // 목표물을 저장할 블랙보드 키
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;
};
