// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateToTarget.h"
#include "AIController.h" // AIController를 사용하기 위해 필요
#include "GameFramework/Pawn.h" // Pawn을 사용하기 위해 필요 (AI가 빙의한 액터)
#include "BehaviorTree/BlackboardComponent.h" // 블랙보드 컴포넌트를 사용하기 위해 필요
#include "Kismet/KismetMathLibrary.h" // 회전 계산을 위해 유용한 함수들
#include "LightHouseDefense/TurretAICharacter.h"
#include "Components/StaticMeshComponent.h" // TurretHead가 UStaticMeshComponent라면 필요
#include "Components/SceneComponent.h"

URotateToTarget::URotateToTarget()
{
	NodeName = TEXT("Rotate To Target");

	bNotifyTick = true;
	bNotifyTaskFinished = true;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(URotateToTarget, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type URotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    //ai컨틀롤러 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;
    //컨트롤러가 빙의 한 폰 가져오기 
    ATurretAICharacter* Turret = Cast<ATurretAICharacter>(AIController->GetPawn());
    if (!Turret) return EBTNodeResult::Failed;

    // 터렛의 기본 스캔 회전 비활성화
    Turret->bIsScanning = false;

    // 태스크가 완료될 때까지 InProgress 상태로 유지
    return EBTNodeResult::InProgress;
}
void URotateToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    ATurretAICharacter* Turret = Cast<ATurretAICharacter>(AIController->GetPawn());
    if (!Turret) {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor) {
        // 타겟이 유효하지 않으면 실패하고 다음 행동으로
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 터렛 헤드의 현재 회전과 타겟을 향하는 회전 계산
    FVector CurrentHeadLocation = Turret->TurretHead->GetComponentLocation();
    FVector TargetLocation = TargetActor->GetActorLocation();
    // Replace the line causing the error with the following code:
    FRotator CurrentRotation = Turret->TurretHead->GetComponentRotation(); // Use GetComponentRotation instead of GetWorldRotation

    // 타겟을 향하는 이상적인 회전
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CurrentHeadLocation, TargetLocation);

    // 부드럽게 회전 보간 (예: 초당 50도)
    float RotationSpeed = 50.0f; // 회전 속도 (도/초)
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed);

    // Z축 (Yaw)만 회전하도록 제한
    // NewRotation.Pitch = CurrentRotation.Pitch;
    // NewRotation.Roll = CurrentRotation.Roll;
    Turret->TurretHead->SetWorldRotation(FRotator(CurrentRotation.Pitch, NewRotation.Yaw, CurrentRotation.Roll));
    Turret->Turretneck->SetWorldRotation(FRotator(CurrentRotation.Pitch, NewRotation.Yaw, CurrentRotation.Roll)); // 목도 같이 회전

    // 회전이 거의 목표에 도달했는지 확인
    if (FMath::Abs(FMath::FindDeltaAngleDegrees(NewRotation.Yaw, TargetRotation.Yaw)) < 1.0f) // 1도 이내로 근접했으면
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 태스크 성공적으로 완료
    }
}
