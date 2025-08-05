// Fill out your copyright notice in the Description page of Project Settings.


#include "AZombieAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"//AI 감각 컴포넌트 추가
#include "Perception/AISenseConfig_Sight.h"//AI 시야 감각 설정 추가
#include "Engine/TargetPoint.h"

AAZombieAIController::AAZombieAIController()
{
   

    //AI 감각 컴포넌트 생성(시야감지용)
    SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component")));

    //AI 시야 감지 설정 추가
    UAISenseConfig_Sight* SightConfig = NewObject<UAISenseConfig_Sight>();
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
    }

}
void AAZombieAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    //BehaviorTreeAsset이 할당되어 있다면 실해
    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);

        //행동 트리가 실행되면 Blackboard 컴포넌트가 자동으로 생성
        UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
        if (BlackboardComp)
        {
            //LighthouseTargetPoint 변수가 Blueprint에서 유효하게 할당되었는지 확인
            if (LighthouseTargetPoint)
            {
                //블랙보드의 "TargetLocation"키에 등대 위치를 저장
                BlackboardComp->SetValueAsVector(TEXT("TargetLocation"), LighthouseTargetPoint->GetActorLocation());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("LighthouseTargetpoint가 할당되지 않았습니다. blueprint에서 할당해주세요."));
            }
        }
    }
    //AIPerception 컴포넌트의 감지 이벤트에 함수를 바인딩
    if (GetPerceptionComponent())
    {
        GetPerceptionComponent()->OnTargetPerceptionUpdated.RemoveDynamic(this, &AAZombieAIController::OnTargetPerceptionUpdated);
        GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAZombieAIController::OnTargetPerceptionUpdated);
    }
}
void AAZombieAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    //AI 컨트롤러가 파괴될 때 델리게이트를 안전하게 제거
    if (GetPerceptionComponent())
    {
        GetPerceptionComponent()->OnTargetPerceptionUpdated.RemoveDynamic(this, &AAZombieAIController::OnTargetPerceptionUpdated);
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
            if (Blackboard)
            {
                Blackboard->SetValueAsObject(TEXT("TargetActor"), Actor);
                UE_LOG(LogTemp, Log, TEXT("Player detected: %s"), *Actor->GetName());
            }

        }
        else //플레이어를 놓쳤다면
        {
            if (Blackboard && Blackboard->GetValueAsObject(TEXT("TargetActor")) == Actor)//블랙보드의 "TargetActor"가 현재 감지된 Actor와 같다면
            {
                Blackboard->ClearValue(TEXT("TargetActor"));//블랙보드에서 "TargetActor" 값을 제거
                UE_LOG(LogTemp, Log, TEXT("Player lost: %s"), *Actor->GetName())
                    //TargetActor를 잃었다면 더이상 추적 행동을 중단하고 다른행동을 한다.
            }
        }
    }
}
