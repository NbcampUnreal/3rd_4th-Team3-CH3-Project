// Fill out your copyright notice in the Description page of Project Settings.


#include "AZombieAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"//AI 감각 컴포넌트 추가
#include "Perception/AISenseConfig_Sight.h"//AI 시야 감각 설정 추가

AAZombieAIController::AAZombieAIController()
{
    //블랙보드 컴포넌트 생성
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

    //AI 감각 컴포넌트 생성(시야감지용)
    SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component")));

    //AI 시야 감지 설정 추가
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    if (SightConfig)
    {
        SightConfig->SightRadius = 2000.0f; // 시야 반경 설정
        SightConfig->LoseSightRadius = SightConfig->SightRadius + 500.0f; // 시야 상실 반경 설정
        SightConfig->PeripheralVisionAngleDegrees = 90.0f; // 주변 시야 각도 설정
        SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 적 감지 활성화
        SightConfig->DetectionByAffiliation.bDetectFriendlies = false; //아군 감지 안함
        SightConfig->DetectionByAffiliation.bDetectNeutrals = false; // 중립 감지 안함

        //AI 감각 컴포넌트에 시야 감각 추가
        GetPerceptionComponent()->ConfigureSense(*SightConfig);
        GetPerceptionComponent()->SetDominantSense(SightConfig->GetClass());
        GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAZombieAIController::OnTargetPerceptionUpdated);
    }

}
void AAZombieAIController::BeginPlay()
{
    Super::BeginPlay();

    //BehaviorTreeAsset이 할당되어 있다면 실해
    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

void AAZombieAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
//플레이어 캐릭터인지 확인
{
    //APawn은 나중에 캐릭터 클래스 이름으로 변경
    if (Actor && Actor->IsA(APawn::StaticClass()) && Actor->GetInstigatorController()->IsPlayerController())
    {
        if (Stimulus.WasSuccessfullySensed())//플레이어를 감지했다면
        {

            //블랙보드에 플레이어를 타겟으로 설정
            if (BlackboardComponent)
            {
                BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), Actor);
                UE_LOG(LogTemp, Log, TEXT("Player detected: %s"), *Actor->GetName());
            }

        }
        else //플레이어를 놓쳤다면
        {
            if (BlackboardComponent && BlackboardComponent->GetValueAsObject(TEXT("TargetActor")) == Actor)//블랙보드의 "TargetActor"가 현재 감지된 Actor와 같다면
            {
                BlackboardComponent->ClearValue(TEXT("TargetActor"));//블랙보드에서 "TargetActor" 값을 제거
                UE_LOG(LogTemp, Log, TEXT("Player lost: %s"), *Actor->GetName())
                    //TargetActor를 잃었다면 더이상 추적 행동을 중단하고 다른행동을 한다.
            }
        }
    }
}
