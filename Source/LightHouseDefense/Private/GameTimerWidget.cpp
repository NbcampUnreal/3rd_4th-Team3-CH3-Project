#include "GameTimerWidget.h"
#include "Components/TextBlock.h"
#include "LighthouseGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h" // [ADDED]

void UGameTimerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ALighthouseGameState* GS = Cast<ALighthouseGameState>(UGameplayStatics::GetGameState(this)))
    {
        // FIX: 중복 바인딩 방지 (PIE 재시작 누적 방지하고 싶으면 AddUniqueDynamic 사용)
        GS->OnTimeUpdated.RemoveDynamic(this, &UGameTimerWidget::UpdateTimeText); // [ADDED]
        GS->OnTimeUpdated.AddUniqueDynamic(this, &UGameTimerWidget::UpdateTimeText);

        // FIX: 초기값 즉시 반영
        UpdateTimeText(GS->GetRemainingTime());
    }
}

// [ADDED] PIE 종료/재시작 시 바인딩 해제
void UGameTimerWidget::NativeDestruct()
{
    if (ALighthouseGameState* GS = Cast<ALighthouseGameState>(UGameplayStatics::GetGameState(this))) // [ADDED]
    {                                                                                                 // [ADDED]
        GS->OnTimeUpdated.RemoveDynamic(this, &UGameTimerWidget::UpdateTimeText);                     // [ADDED]
    }                                                                                                 // [ADDED]
    Super::NativeDestruct();
}

void UGameTimerWidget::UpdateTimeText(int32 RemainingSeconds)
{
    // [NEW] 캐시 갱신
    CachedRemainingSeconds = RemainingSeconds; // [NEW]

    const int32 Minutes = RemainingSeconds / 60;
    const int32 Seconds = RemainingSeconds % 60;

    if (TimerText)
    {
        TimerText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
    }

    // [NEW] 60초 이하로 "처음 진입"했을 때 한 번만 브로드캐스트
    if (!bOneMinuteEventFired && RemainingSeconds <= 60) // [NEW]
    {                                                    // [NEW]
        bOneMinuteEventFired = true;                     // [NEW]
        OnOneMinuteLeft.Broadcast();                     // [NEW]
        UE_LOG(LogTemp, Log, TEXT("[TimerHUD] <= 60s → OnOneMinuteLeft.Broadcast()")); // [NEW]
    }                                                    // [NEW]

    // [ADDED] 0초가 되면 한 번만 GameClear UI 표시
    if (!bGameClearShown && RemainingSeconds <= 0)
    {
        bGameClearShown = true; // [ADDED]
        UE_LOG(LogTemp, Warning, TEXT("[TimerHUD] time==0 → ShowGameClearUI"));
        ShowGameClearUI(); // [ADDED]
    }
}

// [ADDED] 게임 클리어 UI 생성 및 표시
void UGameTimerWidget::ShowGameClearUI()
{
    if (!GameClearWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameClearWidgetClass not set"));
        return;
    }

    UUserWidget* W = CreateWidget<UUserWidget>(GetWorld(), GameClearWidgetClass);
    if (!W) return;

    W->AddToViewport(100); // ZOrder 높게

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        UGameplayStatics::SetGamePaused(this, true); // [ADDED]
        PC->bShowMouseCursor = true; // [ADDED]

        FInputModeUIOnly Mode; // [ADDED]
        Mode.SetWidgetToFocus(W->TakeWidget()); // [ADDED]
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // [ADDED]
        PC->SetInputMode(Mode); // [ADDED]
    }
}
