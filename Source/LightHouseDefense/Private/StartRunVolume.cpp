#include "StartRunVolume.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "LightHouseGameModeBase.h"
#include "GameFramework/Pawn.h"
#include "InputCoreTypes.h"

AStartRunVolume::AStartRunVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    RootComponent = Box;
    Box->InitBoxExtent(FVector(150.f));
    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Box->SetGenerateOverlapEvents(true);
}

void AStartRunVolume::BeginPlay()
{
    Super::BeginPlay();
    Box->OnComponentBeginOverlap.AddDynamic(this, &AStartRunVolume::OnBegin);
    Box->OnComponentEndOverlap.AddDynamic(this, &AStartRunVolume::OnEnd);
}

void AStartRunVolume::OnBegin(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (!Other) return;

    APawn* Pawn = Cast<APawn>(Other);
    if (!Pawn || !Pawn->IsPlayerControlled()) return;

    bPlayerInside = true;

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        EnableInput(PC);
        if (InputComponent)
        {
            // FIX: 액션 매핑 없이 바로 F키 바인딩
            InputComponent->BindKey(EKeys::F, IE_Pressed, this, &AStartRunVolume::HandleStartPressed);
        }
    }
}

void AStartRunVolume::OnEnd(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32)
{
    if (!Other) return;

    APawn* Pawn = Cast<APawn>(Other);
    if (!Pawn || !Pawn->IsPlayerControlled()) return;

    bPlayerInside = false;

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        DisableInput(PC); // 영역 벗어나면 입력 비활성화
    }
}

void AStartRunVolume::HandleStartPressed()
{
    if (!bPlayerInside) return;

    if (ALightHouseGameModeBase* GM = GetWorld()->GetAuthGameMode<ALightHouseGameModeBase>())
    {
        GM->StartRun(); // FIX: 타이머/스폰 시작 트리거
        UE_LOG(LogTemp, Log, TEXT("[StartRunVolume] F pressed → StartRun()"));
    }
}
