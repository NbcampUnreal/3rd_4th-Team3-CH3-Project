#include "BT_ScanForTargets.h"
#include "AITurretPawn.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"

UBT_ScanForTargets::UBT_ScanForTargets()
{
    NodeName = TEXT("Scan For Targets");
    bNotifyTick = true;
    // 블랙보드 키를 오브젝트 타입으로 설정
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBT_ScanForTargets, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBT_ScanForTargets::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(OwnerComp.GetAIOwner()->GetPawn());

    if (!TurretPawn || TurretPawn->CurrentState == ETurretState::Disabled)
    {
        return EBTNodeResult::Failed;
    }

    // 터렛 상태를 스캐닝으로 설정
    TurretPawn->SetTurretState(ETurretState::Scanning);

    return EBTNodeResult::InProgress;
}

void UBT_ScanForTargets::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(OwnerComp.GetAIOwner()->GetPawn());
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (!TurretPawn || !BlackboardComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 블랙보드에 목표물이 설정되면 태스크 성공으로 종료
    if (BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName) != nullptr)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // 틱마다 터렛의 스캐닝 로직 실행
    TurretPawn->ScanForTargets(DeltaSeconds);
}
