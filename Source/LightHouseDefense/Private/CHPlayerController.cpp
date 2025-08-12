#include "CHPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"


void ACHPlayerController::OnAssasultRifleAction()
{
}

void ACHPlayerController::OnAssasultRifle2Action()
{
}

void ACHPlayerController::OnPistolAction()
{
}

void ACHPlayerController::OnSinperRifleAction()
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
    // HUD 위젯 생성 및 표시
    if (HUDWidgetClass)
    {
        UUserWidget* HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }
}

void ACHPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInput->BindAction(IA_AssasultRifleAction, ETriggerEvent::Started, this, &ACHPlayerController::OnAssasultRifleAction);
        EnhancedInput->BindAction(IA_AssasultRifle2Action, ETriggerEvent::Started, this, &ACHPlayerController::OnAssasultRifle2Action);
        EnhancedInput->BindAction(IA_PistolAction, ETriggerEvent::Started, this, &ACHPlayerController::OnPistolAction);
        EnhancedInput->BindAction(IA_SinperRifleAction, ETriggerEvent::Started, this, &ACHPlayerController::OnSinperRifleAction);
        EnhancedInput->BindAction(IA_ShotgunAction, ETriggerEvent::Started, this, &ACHPlayerController::OnShotgunAction);
    }
}

