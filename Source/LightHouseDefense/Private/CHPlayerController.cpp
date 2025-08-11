#include "CHPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"


void ACHPlayerController::OnRifleAction()
{
}

void ACHPlayerController::OnShotgunAction()
{
}



ACHPlayerController::ACHPlayerController()
    :InputMappingContext(nullptr),
    MoveAction(nullptr),
    JumpAction(nullptr),
    LookAction(nullptr),
    SprintAction(nullptr),
    CrouchAction(nullptr)
{
}

void ACHPlayerController::BeginPlay()
{
    Super::BeginPlay();


    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                SubSystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }
}

void ACHPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInput->BindAction(IA_RifleAction, ETriggerEvent::Started, this, &ACHPlayerController::OnRifleAction);
        EnhancedInput->BindAction(IA_ShotgunAction, ETriggerEvent::Started, this, &ACHPlayerController::OnShotgunAction);
    }
}

