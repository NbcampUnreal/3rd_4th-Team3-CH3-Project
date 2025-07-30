#include "TurretAICharacter.h"
#include "TurretAIControlller.h"

ATurretAICharacter::ATurretAICharacter()
{
   

    AIControllerClass = ATurretAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
    RootComponent = Root;

    TurretHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
    TurretHead->SetupAttachment(Root);

    TurretBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body")); 
    TurretBody->SetupAttachment(Root);


    Turretneck = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("neck"));
    Turretneck->SetupAttachment(Root);

    TurretBody->SetRelativeLocation(FVector(0, 0, 0));
    TurretHead->SetRelativeLocation(FVector(0, 0, 0));
    Turretneck->SetRelativeLocation(FVector(0, 0, 0));
  
}

void ATurretAICharacter::Tick(float DeltaTime)  
{  
    Super::Tick(DeltaTime);  

    if (bIsScanning) // 스캔 중일 때만 회전
    {
        FRotator scoutHead = TurretHead->GetRelativeRotation();
        scoutHead.Yaw += 90.0f * DeltaTime;
        TurretHead->SetRelativeRotation(scoutHead);

        FRotator scoutneck = Turretneck->GetRelativeRotation();
        scoutneck.Yaw += 90.0f * DeltaTime;
        Turretneck->SetRelativeRotation(scoutneck);
    }
}
