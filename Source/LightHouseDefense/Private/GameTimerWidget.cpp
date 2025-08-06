#include "GameTimerWidget.h"
#include "Components/TextBlock.h"
#include "LighthouseGameState.h"
#include "Kismet/GameplayStatics.h"

void UGameTimerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ALighthouseGameState* GS = Cast<ALighthouseGameState>(UGameplayStatics::GetGameState(this)))
    {
        GS->OnTimeUpdated.AddDynamic(this, &UGameTimerWidget::UpdateTimeText);
        UpdateTimeText(GS->GetRemainingTime());
    }
}

void UGameTimerWidget::UpdateTimeText(int32 RemainingSeconds)
{
    int32 Minutes = RemainingSeconds / 60;
    int32 Seconds = RemainingSeconds % 60;

    FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

    if (TimerText)
    {
        TimerText->SetText(FText::FromString(TimeString));
    }
}
