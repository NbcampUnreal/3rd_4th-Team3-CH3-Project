#include "TurretAIControlller.h"
#include <Perception/AISenseConfig_Sight.h>
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BlackboardComponent.h" // 추가된 헤더 파일

ATurretAIController::ATurretAIController()
{
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));


    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1500.0f;
    SightConfig->LoseSightRadius = 2000.0f;
    SightConfig->PeripheralVisionAngleDegrees = 360.0f;
    SightConfig->SetMaxAge(5.0f);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->SetDominantSense(UAISense_Sight::StaticClass());

    // Blackboard Component 생성
    // 이건 실제 데이터를 담는 실행용 컨테이너 (게임 도중 키/값을 저장)
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));
   

    AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ATurretAIController::OnTargetPerceptionUpdated);


    

}

void ATurretAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


}
void ATurretAIController::BeginPlay()
{
    Super::BeginPlay();

    if (BlackboardComp)
    {
        // 초기값 설정 – 시작할 때 Blackboard에 값 미리 넣어둠
        // BT에서 이 값들을 조건 판단에 사용할 수 있음
        BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false); // 타겟 탐지 여부 초기화
       
        BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false); // 조사 중 상태 초기화
    
        UE_LOG(LogTemp, Warning, TEXT("[Sparta] Blackboard initialized successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Sparta] Blackboard Component not found!"));
    }

   
}

void ATurretAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // 블랙보드 컴포넌트가 유효한지 먼저 확인 (매우 중요)
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Error, TEXT("BlackboardComp가 유효하지 않습니다!"));
        return;
    }
/*  좀비구현이 필요
    // 성공적으로 감지했을 때만 처리
    if (Stimulus.WasSuccessfullySensed())
    {// 인지된 Actor를 좀비 클래스로 캐스팅 시도
        AZombieCharacter* ZombieActor = Cast<AZombieCharacter>(Actor);

        // 캐스팅에 성공했고 (좀비이며), 이 액터가 유효하다면
        if (ZombieActor && IsValid(ZombieActor)) // IsValid()는 액터가 파괴되지 않았는지 확인
        {
            // 적 좀비를 봤다!
            BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), true);
            BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor); // 또는 ZombieActor

            UE_LOG(LogTemp, Warning, TEXT("좀비 타겟 감지됨: %s"), *Actor->GetName());
        }
        else // 좀비가 아니거나 유효하지 않은 액터라면 무시
        {
            // 이미 타겟이 설정되어 있는데, 감지된 것이 좀비가 아니라면 현재 타겟을 유지하거나,
            // 더 복잡한 로직을 추가하여 현재 타겟이 유효한 좀비인지 다시 확인해야 할 수 있습니다.
            // 단순하게는 좀비가 아니면 아무것도 하지 않습니다.
            // 또는, 현재 타겟이 이 액터였는데 좀비가 아니라면 클리어하는 로직을 추가할 수도 있습니다.
            UE_LOG(LogTemp, Warning, TEXT("감지된 액터는 좀비가 아닙니다: %s"), *Actor->GetName());
        }
        // ====================
    }
    else // 대상을 놓쳤을 때
    {
        // 놓친 대상이 현재 블랙보드에 저장된 타겟 액터와 동일한지 확인하는 것이 좋습니다.
        // 다른 액터를 놓쳤는데 엉뚱한 타겟 정보를 지우지 않도록 하기 위함입니다.
        if (BlackboardComp->GetValueAsObject(TEXT("TargetActor")) == Actor)
        {
            // 적을 놓쳤다!
            BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
            BlackboardComp->ClearValue(TEXT("TargetActor"));

            UE_LOG(LogTemp, Warning, TEXT("타겟 놓침: %s"), *Actor->GetName());
        }
    }*/
}