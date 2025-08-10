#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LighthouseHUD.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API ALighthouseHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION() // 반드시 있어야 OnTimeUpdated에 바인딩 가능
        void UpdateTimerText(int32 RemainingTime);

    void UpdateZombieCount(int32 RemainingZombies);

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UUserWidget> GameHUDWidgetClass;

private:
    class UUserWidget* GameHUDWidget;
    class UTextBlock* TimerTextBlock;
    class UTextBlock* ZombieCountTextBlock;
};
