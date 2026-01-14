// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AZombieAIController.generated.h"

class UBehaviorTree;
class ATargetPoint;

UCLASS()
class LIGHTHOUSEDEFENSE_API AAZombieAIController : public AAIController
{
	GENERATED_BODY()

public:
    AAZombieAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    //AI가 사용할 행동 트리 에셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    UBehaviorTree* BehaviorTreeAsset;

    //플레이어를 감지했을 때 호출될 함수
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    //등대 위치를 지정할 TargetPoint를 Blueprint에서 할당
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    ATargetPoint* LighthouseTargetPoint;
};
