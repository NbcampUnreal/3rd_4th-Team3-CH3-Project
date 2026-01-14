#include "CHPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"


void ACHPlayerController::OnAK47Action()
{
}

void ACHPlayerController::OnM16Action()
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
        EnhancedInput->BindAction(IA_AK47Action, ETriggerEvent::Started, this, &ACHPlayerController::OnAK47Action);
        EnhancedInput->BindAction(IA_M16Action, ETriggerEvent::Started, this, &ACHPlayerController::OnM16Action);
        EnhancedInput->BindAction(IA_PistolAction, ETriggerEvent::Started, this, &ACHPlayerController::OnPistolAction);
        EnhancedInput->BindAction(IA_SinperRifleAction, ETriggerEvent::Started, this, &ACHPlayerController::OnSinperRifleAction);
        EnhancedInput->BindAction(IA_ShotgunAction, ETriggerEvent::Started, this, &ACHPlayerController::OnShotgunAction);
    }
}

