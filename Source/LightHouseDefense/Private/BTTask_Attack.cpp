#include "BTTask_Attack.h"
#include "AITurretPawn.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack and Rotate");
    bNotifyTick = true;
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Attack, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // 태스크가 시작될 때 타이머를 InProgress 상태로 둡니다.
    return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(AIController->GetPawn());
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (!TurretPawn || !BlackboardComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 타겟이 유효한지 확인합니다.
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor || !IsValid(TargetActor) || TurretPawn->CurrentState == ETurretState::Disabled)
    {
        // 타겟이 사라지거나 터렛이 비활성화되면 태스크를 실패로 끝냅니다.
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // **회전 로직**
    FVector TargetLocation = TargetActor->GetActorLocation();
    FVector CurrentLocation = TurretPawn->TurretNeck->GetComponentLocation();
    FRotator CurrentRotation = TurretPawn->TurretNeck->GetComponentRotation();

    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, TargetLocation);
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, TurretPawn->RotationSpeed);

  
    TurretPawn->TurretNeck->SetWorldRotation(FRotator(CurrentRotation.Pitch, NewRotation.Yaw, CurrentRotation.Roll));

    // **공격 로직**
    // 아직 타이머가 설정되지 않았다면 공격을 시작합니다.
    if (!TurretPawn->GetWorld()->GetTimerManager().IsTimerActive(TurretPawn->FireTimerHandle))
    {
        TurretPawn->GetWorld()->GetTimerManager().SetTimer(TurretPawn->FireTimerHandle, TurretPawn, &AAITurretPawn::Fire, TurretPawn->FireRate, true);
        TurretPawn->SetTurretState(ETurretState::Attacking);
    }
}

void UBTTask_Attack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)  
{  
    FinishLatentTask(OwnerComp, TaskResult);  

    // 태스크가 종료될 때 공격 타이머를 확실히 중지시킵니다.  
    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(OwnerComp.GetAIOwner()->GetPawn());  
    if (TurretPawn)  
    {  
        TurretPawn->GetWorld()->GetTimerManager().ClearTimer(TurretPawn->FireTimerHandle);  
    }  
}
