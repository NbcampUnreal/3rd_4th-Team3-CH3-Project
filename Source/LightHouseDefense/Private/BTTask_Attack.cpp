#include "BTTask_Attack.h"
#include "AITurretPawn.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack");
    bNotifyTick = true;
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Attack, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(OwnerComp.GetAIOwner()->GetPawn());
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (!TurretPawn || !BlackboardComp || TurretPawn->CurrentState == ETurretState::Disabled)
    {
        return EBTNodeResult::Failed;
    }

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor || !IsValid(TargetActor))
    {
        return EBTNodeResult::Failed;
    }

    // 공격 상태로 변경하고 발사 타이머 시작
    TurretPawn->SetTurretState(ETurretState::Attacking);
    TurretPawn->GetWorld()->GetTimerManager().SetTimer(TurretPawn->FireTimerHandle, TurretPawn, &AAITurretPawn::Fire, TurretPawn->FireRate, true);

    return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(OwnerComp.GetAIOwner()->GetPawn());
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (!TurretPawn || !BlackboardComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

    if (!TargetActor || !IsValid(TargetActor) || TurretPawn->CurrentState == ETurretState::Disabled)
    {
        // 목표가 사라지거나 비활성화되면 타이머 중단 및 태스크 실패
        TurretPawn->GetWorld()->GetTimerManager().ClearTimer(TurretPawn->FireTimerHandle);
        BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName); // 블랙보드에서 타겟 정보 제거

        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
}
