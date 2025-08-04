// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTurretAttack.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "LightHouseDefense/TurretAICharacter.h"
#include "DrawDebugHelpers.h" // 디버그 라인/구체 사용 시 필요
#include "Components/SceneComponent.h" // GetComponentLocation/Rotation 사용 시 필요


UBTTurretAttack:: UBTTurretAttack()
{
    NodeName = TEXT("Attack Target");

    bNotifyTick = true;

    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTurretAttack, TargetActorKey), AActor::StaticClass());


    AttackInterval = 0.5f;
    AttackDamage = 10.0f;
    LineTraceRange = 2000.0f;
    MuzzleComponentName = TEXT("Muzzle"); // 기본 총구 컴포넌트 이름
}


void UBTTurretAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    //AI컨트롤러 가져오기 없으면 테스크 종료
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    //터렛 가져오기 없으면 테스크 종료
    ATurretAICharacter* Turret = Cast<ATurretAICharacter>(AIController->GetPawn());
    if (!Turret)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    //블랙보드 가져오기 없으면 테스크 종료
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {

        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    // 1. 타겟 유효성 검사 (죽었는지, 사라졌는지 등)
// 좀비가 죽었는지 확인하려면 좀비 클래스에 체력이나 IsDead() 함수가 있어야 합니다.
// 여기서는 일단 액터가 유효한지(null이 아니고, Destroyed되지 않았는지)만 확인합니다.
// 만약 좀비 클래스에 FHealthComponent 같은 것이 있다면 추가 검사 필요.
    bool bTargetIsDead = false; // TODO: 여기에 좀비의 죽음 여부를 확인하는 로직 추가
    // AZombieCharacter* Zombie = Cast<AZombieCharacter>(TargetActor);
    //     if (Zombie && Zombie->IsDead()) { bTargetIsDead = true; }

    if (!TargetActor || !IsValid(TargetActor) || TargetActor->IsPendingKillPending() || bTargetIsDead)
    {
        // 타겟이 유효하지 않거나 죽었다면 태스크 성공으로 종료하여 다음 타겟을 찾게 함.
        // 블랙보드의 CanSeeTarget 키를 false로 설정하여 AI가 다음 행동으로 넘어가게 합니다.
        BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false); // 중요: 타겟을 잃었다고 AI에게 알림
        BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName); // 타겟 액터 블랙보드 값 클리어
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // 2. 공격 쿨다운 확인
    if (GetWorld()->GetTimeSeconds() - LastAttackTime >= AttackInterval)
    {
        // 3. 공격 로직: 라인 트레이스
        FVector StartLocation;
        FRotator AttackRotation;

        // 총구 위치와 회전 가져오기
        USceneComponent* MuzzleComp = Turret->MuzzleComponent;
        if (MuzzleComp)
        {
            StartLocation = MuzzleComp->GetComponentLocation();
            AttackRotation = MuzzleComp->GetComponentRotation(); // 총구는 이미 RotateToTarget에 의해 타겟을 바라보고 있을 것임
        }
        else
        {
            // 총구 컴포넌트가 없으면 터렛 헤드의 위치를 사용
            StartLocation = Turret->TurretHead->GetComponentLocation();
            AttackRotation = Turret->TurretHead->GetComponentRotation();
        }

        FVector EndLocation = StartLocation + AttackRotation.Vector() * LineTraceRange;

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Turret); // 터렛 자신은 무시


        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1, Params); // Custom Trace Channel 사용할 수도 있음

        // DEBUG: 라인 트레이스 시각화 (인게임에서 확인용)
#if ENABLE_DRAW_DEBUG
        DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.1f, 0, 2.0f);
        if (bHit)
        {
            DrawDebugSphere(GetWorld(), HitResult.Location, 10.0f, 12, FColor::Green, false, 0.1f, 0, 2.0f);
        }
#endif

        if (bHit)
        {
            // 4. 데미지 적용
            AActor* HitActor = HitResult.GetActor();
            if (HitActor && HitActor == TargetActor) // 명중한 액터가 우리가 노리던 타겟 좀비라면
            {
                UGameplayStatics::ApplyDamage(HitActor, AttackDamage, AIController, Turret, UDamageType::StaticClass());
                UE_LOG(LogTemp, Warning, TEXT("터렛이 %s에게 %f 데미지 공격!"), *HitActor->GetName(), AttackDamage);
                // TODO: 공격 이펙트, 사운드 재생 등
            }
        }

        LastAttackTime = GetWorld()->GetTimeSeconds(); // 마지막 공격 시간 업데이트
    }

    // 태스크는 좀비가 죽을 때까지 InProgress 상태를 유지합니다.
    // 좀비가 죽으면 위에서 FinishLatentTask(EBTNodeResult::Succeeded)가 호출될 것입니다.

}

EBTNodeResult::Type UBTTurretAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // TickTask()에서 대부분의 작업을 하고 있기 때문에, 그냥 InProgress로 반환
    return EBTNodeResult::InProgress;
}
