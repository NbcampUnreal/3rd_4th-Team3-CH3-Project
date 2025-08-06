
#include "TurretAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AZombieCharacter.h"
#include "AITurretPawn.h"
#include "Perception/AIPerceptionTypes.h"

ATurretAIController::ATurretAIController()
{
    // 생성자에서는 컴포넌트를 생성하고, 기본 프로퍼티를 설정합니다.
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

    // UAISenseConfig_Sight 객체만 생성하여 멤버 변수에 할당
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

    if (SightConfig)
    {
        SightConfig->SightRadius = 2500.0f;
        SightConfig->LoseSightRadius = 3000.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    }

    SetGenericTeamId(FGenericTeamId(2));
}

void ATurretAIController::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay()에서 AI Perception 컴포넌트 설정
    UAIPerceptionComponent* MyPerceptionComponent = GetPerceptionComponent();
    if (PerceptionComponent && SightConfig)
    {
        PerceptionComponent->ConfigureSense(*SightConfig);
        PerceptionComponent->SetDominantSense(SightConfig->GetClass());
        PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ATurretAIController::OnTargetPerceptionUpdated);
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
