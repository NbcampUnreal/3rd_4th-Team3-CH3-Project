#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <Perception/AIPerceptionTypes.h>
#include "TurretAIController.generated.h"

// 전방 선언
class UBehaviorTree;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class LIGHTHOUSEDEFENSE_API ATurretAIController : public AAIController
{
    GENERATED_BODY()

public:
    ATurretAIController();

protected:
    // virtual 키워드를 명시하여 재정의임을 더 명확히 합니다.
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardComponent* BlackboardComponent;

  
   

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;


    // 시야 감지 설정 객체를 멤버 변수로 선언
    UPROPERTY(VisibleAnywhere, Category = "AI")
    UAISenseConfig_Sight* SightConfig;

    // 시야 감지 업데이트 이벤트에 바인딩할 함수
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
