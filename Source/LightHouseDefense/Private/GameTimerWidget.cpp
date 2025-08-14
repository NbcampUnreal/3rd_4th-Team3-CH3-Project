#include "GameTimerWidget.h"
#include "Components/TextBlock.h"
#include "LighthouseGameState.h"
#include "Kismet/GameplayStatics.h"

void UGameTimerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ALighthouseGameState* GS = Cast<ALighthouseGameState>(UGameplayStatics::GetGameState(this)))
    {
        // FIX: 중복 바인딩 방지 (PIE 재시작 누적 방지하고 싶으면 AddUniqueDynamic 사용)
        GS->OnTimeUpdated.AddUniqueDynamic(this, &UGameTimerWidget::UpdateTimeText);

        // FIX: 초기값 즉시 반영
        UpdateTimeText(GS->GetRemainingTime());
    }
}

void UGameTimerWidget::UpdateTimeText(int32 RemainingSeconds)
{
    const int32 Minutes = RemainingSeconds / 60;
    const int32 Seconds = RemainingSeconds % 60;

    if (TimerText)
    {
        TimerText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
    }
}
