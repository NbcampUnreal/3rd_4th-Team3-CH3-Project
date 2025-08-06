#include "LighthouseHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"

void ALighthouseHUD::BeginPlay()
{
    Super::BeginPlay();

    if (GameHUDWidgetClass)
    {
        UE_LOG(LogTemp, Log, TEXT("Creating GameHUDWidget."));
        GameHUDWidget = CreateWidget<UUserWidget>(GetWorld(), GameHUDWidgetClass);
        if (GameHUDWidget)
        {
            GameHUDWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("GameHUDWidget added to viewport."));

            TimerTextBlock = Cast<UTextBlock>(GameHUDWidget->GetWidgetFromName(TEXT("TimerText")));
            ZombieCountTextBlock = Cast<UTextBlock>(GameHUDWidget->GetWidgetFromName(TEXT("ZombieCountText")));

            if (!TimerTextBlock)
                UE_LOG(LogTemp, Warning, TEXT("TimerTextBlock not found! Check widget hierarchy and name."));
            if (!ZombieCountTextBlock)
                UE_LOG(LogTemp, Warning, TEXT("ZombieCountTextBlock not found! Check widget hierarchy and name."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create GameHUDWidget."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameHUDWidgetClass is not assigned in HUD."));
    }
}

void ALighthouseHUD::UpdateTimerText(int32 RemainingTime)
{
    UE_LOG(LogTemp, Log, TEXT("UpdateTimerText called with RemainingTime: %d"), RemainingTime);

    if (TimerTextBlock)
    {
        int32 Minutes = RemainingTime / 60;
        int32 Seconds = RemainingTime % 60;
        FString TimeStr = FString::Printf(TEXT("Time Left: %02d:%02d"), Minutes, Seconds);
        TimerTextBlock->SetText(FText::FromString(TimeStr));
    }
}

void ALighthouseHUD::UpdateZombieCount(int32 RemainingZombies)
{
    if (ZombieCountTextBlock)
    {
        FString CountStr = FString::Printf(TEXT("Zombies Left: %d"), RemainingZombies);
        ZombieCountTextBlock->SetText(FText::FromString(CountStr));
    }
}
