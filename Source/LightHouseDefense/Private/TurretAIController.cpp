#include "TurretAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AZombieCharacter.h"
#include "AITurretPawn.h"

ATurretAIController::ATurretAIController()
{
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

    // SightConfig는 부모 클래스에 없으므로 여기서 생성합니다.
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

    // GetPerceptionComponent()를 사용해 부모의 컴포넌트에 접근합니다.
    UAIPerceptionComponent* MyPerceptionComponent = GetPerceptionComponent();
    if (MyPerceptionComponent && SightConfig)
    {
        SightConfig->SightRadius = 2500.0f;
        SightConfig->LoseSightRadius = 3000.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

        MyPerceptionComponent->ConfigureSense(*SightConfig);
    }

    SetGenericTeamId(FGenericTeamId(2));
}

void ATurretAIController::BeginPlay()
{
    Super::BeginPlay();

    UAIPerceptionComponent* MyPerceptionComponent = GetPerceptionComponent();
    if (MyPerceptionComponent && SightConfig)
    {
        MyPerceptionComponent->SetDominantSense(SightConfig->GetClass());
        MyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ATurretAIController::OnTargetPerceptionUpdated);
    }

    AAITurretPawn* TurretPawn = Cast<AAITurretPawn>(GetPawn());
    if (TurretPawn && TurretPawn->BehaviorTreeAsset)
    {
        if (TurretPawn->CurrentState != ETurretState::Disabled)
        {
            RunBehaviorTree(TurretPawn->BehaviorTreeAsset);
        }
    }
}

void ATurretAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
    if (!BlackboardComp) return;

    AAZombieCharacter* ZombieCharacter = Cast<AAZombieCharacter>(Actor);
    if (ZombieCharacter && Stimulus.WasSuccessfullySensed())
    {
        BlackboardComp->SetValueAsObject(TEXT("TargetActor"), ZombieCharacter);
    }
    else if (ZombieCharacter && !Stimulus.WasSuccessfullySensed())
    {
        BlackboardComp->ClearValue(TEXT("TargetActor"));
    }
}
