#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "TurretAIController.generated.h"

// 전방 선언
class UBehaviorTree;
class UBlackboardComponent;
class UAISenseConfig_Sight;

UCLASS()
class LIGHTHOUSEDEFENSE_API ATurretAIController : public AAIController
{
    GENERATED_BODY()

public:
    ATurretAIController();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardComponent* BlackboardComponent;

    // 여기에 PerceptionComponent 변수를 선언하지 마세요! 부모 클래스에 이미 있습니다.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    // 시야 감지 설정 객체를 멤버 변수로 선언
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
    UAISenseConfig_Sight* SightConfig;

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
