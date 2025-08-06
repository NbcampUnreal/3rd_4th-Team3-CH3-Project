#include "LighthouseGameState.h"
#include "Kismet/GameplayStatics.h"
#include "LighthouseHUD.h"
#include "Engine/World.h"
#include "TimerManager.h"

void ALighthouseGameState::BeginPlay()
{
    Super::BeginPlay();
    StartCountdown();

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        ALighthouseHUD* LH_HUD = Cast<ALighthouseHUD>(PC->GetHUD());
        if (LH_HUD)
        {
            OnTimeUpdated.AddDynamic(LH_HUD, &ALighthouseHUD::UpdateTimerText);
        }
    }
}


void ALighthouseGameState::StartCountdown()
{
    GetWorldTimerManager().SetTimer(
        CountdownTimerHandle,
        this,
        &ALighthouseGameState::UpdateTimer,
        1.0f,
        true
    );
}

void ALighthouseGameState::UpdateTimer()
{
    RemainingTime--;

    if (RemainingTime <= 0)
    {
        RemainingTime = 0;
        GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
    }

    OnTimeUpdated.Broadcast(RemainingTime);
}

int32 ALighthouseGameState::GetRemainingTime() const
{
    return RemainingTime;
}
