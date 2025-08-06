#include "BTTask_RotateToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AITurretPawn.h"
#include "Components/StaticMeshComponent.h"


UBTTask_RotateToTarget::UBTTask_RotateToTarget()
{
    NodeName = TEXT("Rotate To Target");
    bNotifyTick = true;
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_RotateToTarget, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(OwnerComp.GetAIOwner()->GetPawn());
    if (!TurretPawn || TurretPawn->CurrentState == ETurretState::Disabled)
    {
        return EBTNodeResult::Failed;
    }
   
    // TickTask에서 회전 로직을 처리하므로 InProgress 반환
    return EBTNodeResult::InProgress;
}

void UBTTask_RotateToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    
    //터랫 가져오기
    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(OwnerComp.GetAIOwner()->GetPawn());
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    //터랫에 블랙보드가 없거나 비활성화 상태라면 작동 안됨 
    if (!TurretPawn || !BlackboardComp || TurretPawn->CurrentState == ETurretState::Disabled)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    //타겟 가지고 오기 
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor || !IsValid(TargetActor))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    // 회전 로직
    FVector TargetLocation = TargetActor->GetActorLocation();
    FVector CurrentLocation = TurretPawn->TurretHead->GetComponentLocation();
    FRotator CurrentRotation = TurretPawn->TurretHead->GetComponentRotation();

    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, TargetLocation);
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, TurretPawn->RotationSpeed);

    // Z축 회전만 적용
    // TurretHeadPivot를 회전시키면 하위 컴포넌트들도 함께 회전합니다.
    TurretPawn->TurretHead->SetWorldRotation(FRotator(CurrentRotation.Pitch, NewRotation.Yaw, CurrentRotation.Roll));
    TurretPawn->TurretNeck->SetWorldRotation(FRotator(CurrentRotation.Pitch, NewRotation.Yaw, CurrentRotation.Roll));

    // 회전이 거의 완료되면 태스크 성공 처리
    if (FMath::Abs(FMath::FindDeltaAngleDegrees(NewRotation.Yaw, TargetRotation.Yaw)) < 1.0f)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
