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
    // 1. 터렛 폰과 블랙보드 컴포넌트 유효성 검사
    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(OwnerComp.GetAIOwner()->GetPawn());
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (!TurretPawn || !BlackboardComp || TurretPawn->CurrentState == ETurretState::Disabled)
    {
        return EBTNodeResult::Failed;
    }

    // 2. 타겟 액터 유효성 검사
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor || !TargetActor->IsValidLowLevel())
    {
        return EBTNodeResult::Failed;
    }

    // 3. 공격 상태로 변경하고 발사 타이머 시작
    TurretPawn->CurrentState = ETurretState::Attacking;
    TurretPawn->GetWorld()->GetTimerManager().SetTimer(TurretPawn->FireTimerHandle, TurretPawn, &AAITurretPawn::Fire, TurretPawn->FireRate, true);

    // 4. TickTask를 위해 InProgress 반환
    return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    // 1. 터렛 폰과 블랙보드 컴포넌트 유효성 검사 (ExecuteTask와 동일)
    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(OwnerComp.GetAIOwner()->GetPawn());
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    // 2. TurretPawn과 BlackboardComp가 유효하지 않으면 즉시 종료
    if (!TurretPawn || !BlackboardComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 3. 타겟 액터 유효성 검사
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

    // 4. 타겟이 유효하지 않거나, 터렛이 비활성화되면 태스크 실패
    if (!TargetActor || !TargetActor->IsValidLowLevel() || TurretPawn->CurrentState == ETurretState::Disabled)
    {
        // 공격 상태를 스캔 상태로 되돌리고 타이머를 클리어
        TurretPawn->CurrentState = ETurretState::Scanning;
        TurretPawn->GetWorld()->GetTimerManager().ClearTimer(TurretPawn->FireTimerHandle);
        BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName);

        // 태스크 실패로 종료
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
}
