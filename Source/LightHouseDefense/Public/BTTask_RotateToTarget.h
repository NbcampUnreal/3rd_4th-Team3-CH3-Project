#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToTarget.generated.h"

UCLASS(Blueprintable)
class LIGHTHOUSEDEFENSE_API UBTTask_RotateToTarget : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_RotateToTarget();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;

private:
    bool IsTargetRotated(class ATurretAICharacter* TurretPawn, AActor* TargetActor);
};
