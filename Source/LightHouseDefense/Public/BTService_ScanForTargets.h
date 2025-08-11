#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_ScanForTargets.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API UBTService_ScanForTargets : public UBTService_BlackboardBase
{
    GENERATED_BODY()

public:
    UBTService_ScanForTargets();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
