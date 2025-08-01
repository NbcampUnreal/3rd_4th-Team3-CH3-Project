// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AZombieAIController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;

UCLASS()
class LIGHTHOUSEDEFENSE_API AAZombieAIController : public AAIController
{
	GENERATED_BODY()

public:
    AAZombieAIController();

protected:
    virtual void BeginPlay() override;//AI 컨트롤러가 월드에 나타날때 호출

public:
    //AI가 사용할 행동 트리 에셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    UBehaviorTree* BehaviorTreeAsset;

    //AI가 사용할 블랙보드 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category="AI")
    UBlackboardComponent* BlackboardComponent;

    //플레이어를 감지했을 때 호출될 함수
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
