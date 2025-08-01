// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack Player");
    AttackDuration = 1.0f; // 기본 공격 시간
    AttackDamage = 10.0f; // 기본 공격 데미지
    AttackRange = 150.0f; // 기본 공격 범위 (캐릭터 캡슐 크기에 따라 조절)
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if(!AICharacter || !BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if(!TargetActor)
    {
        return EBTNodeResult::Failed;
    }

    //타겟과의 거리 체크
    float DistanceToTarget = FVector::Dist(AICharacter->GetActorLocation(), TargetActor->GetActorLocation());
    if (DistanceToTarget > AttackRange)
    {
        //타겟이 공격 범위를 벗어났다면 실패
        return EBTNodeResult::Failed;
    }
    //공격 애니메이션 재생

    //데미지 적용 로직
    UGameplayStatics::ApplyDamage(TargetActor, AttackDamage, AIController, AICharacter, UDamageType::StaticClass());
    UE_LOG(LogTemp, Warning, TEXT("Zombie %s attacked %s for %.1f damage!"), *AICharacter->GetName(), *TargetActor->GetName(), AttackDamage);

    //task 성공 및 완료 대기(AttackDuration 초 동안)
    //FinishLatentTask를 사용하려면 bNotifyTaskFinished를 true로 설정해야 합니다.
    return EBTNodeResult::InProgress;

}
