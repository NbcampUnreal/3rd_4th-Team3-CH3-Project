#include "BTService_ScanForTargets.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AZombieCharacter.h"
#include "AITurretPawn.h"

UBTService_ScanForTargets::UBTService_ScanForTargets()
{
    // 서비스 노드의 이름을 지정합니다.
    NodeName = TEXT("Scan For Targets");
    // 서비스 노드가 매 프레임 실행되도록 설정
    bNotifyTick = true;
    // Tick 간격을 0.2초로 설정하여 너무 자주 실행되지 않도록 합니다.
    Interval = 0.2f;
    RandomDeviation = 0.1f;
}

void UBTService_ScanForTargets::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // AI 컨트롤러와 폰을 가져옵니다.
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    APawn* TurretPawn = AIController->GetPawn();
    if (!TurretPawn) return;

    // 터렛 주변 모든 좀비를 찾습니다.
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAZombieCharacter::StaticClass(), FoundActors);

    AActor* ClosestTarget = nullptr;
    float ClosestDistanceSq = TNumericLimits<float>::Max();
    FVector TurretLocation = TurretPawn->GetActorLocation();

    // 가장 가까운 좀비를 찾습니다.
    for (AActor* Zombie : FoundActors)
    {
        if (Zombie && IsValid(Zombie))
        {
            float DistanceSq = FVector::DistSquared(TurretLocation, Zombie->GetActorLocation());
            if (DistanceSq < ClosestDistanceSq)
            {
                ClosestDistanceSq = DistanceSq;
                ClosestTarget = Zombie;
            }
        }
    }

    // 찾은 대상을 블랙보드에 설정하거나, 없으면 지웁니다.
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {
        if (ClosestTarget)
        {
            // 블랙보드 키에 가장 가까운 좀비를 할당합니다.
            BlackboardComp->SetValueAsObject(GetSelectedBlackboardKey(), ClosestTarget);
        }
        else
        {
            // 좀비가 없다면 블랙보드 키를 지웁니다.
            BlackboardComp->ClearValue(GetSelectedBlackboardKey());
        }
    }
}
