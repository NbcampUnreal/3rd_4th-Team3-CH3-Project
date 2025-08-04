// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include <Perception/AISenseConfig_Sight.h>
#include "BehaviorTree/BehaviorTree.h"
#include "TurretAIControlller.generated.h"


/**
 * 
 */
UCLASS()
class LIGHTHOUSEDEFENSE_API ATurretAIController : public AAIController
{
	GENERATED_BODY()

public:
	ATurretAIController();
	// Behavior Tree 시작 함수
	void StartBehaviorTree();

	// Getter 함수 – 외부에서 BlackboardComp에 접근할 수 있게 해줌
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const
	{
		return BlackboardComp;
	}

protected:

	// Behavior Tree 에셋 참조
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;

	// [Blackboard Component] : 실제 실행 중 데이터를 저장하는 "기억장치"
	// 실제 값들을 들고 있음 (Key에 해당하는 실시간 값 저장소)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComp;


	//터렛의 시야
// AI Perception 시스템의 핵심 컴포넌트로, 이 액터가 감지 시스템(시야, 청각 등)을 통해
// 주변 환경을 인식할 수 있게 해주는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;
	//시야(Sight) 감각을 담당하는 구성
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;


	
};

