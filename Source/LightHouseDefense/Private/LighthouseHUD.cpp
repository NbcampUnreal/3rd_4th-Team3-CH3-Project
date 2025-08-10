#include "LighthouseHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"

// 게임 시작 시 HUD 초기화
void ALighthouseHUD::BeginPlay()
{
    Super::BeginPlay();

    // HUD 위젯 클래스가 지정되어 있는지 확인
    if (GameHUDWidgetClass)
    {
        UE_LOG(LogTemp, Log, TEXT("Creating GameHUDWidget."));
        // 지정된 UUserWidget 클래스로 HUD 생성
        GameHUDWidget = CreateWidget<UUserWidget>(GetWorld(), GameHUDWidgetClass);
        if (GameHUDWidget)
        {
            // 화면에 위젯 표시
            GameHUDWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("GameHUDWidget added to viewport."));
            // 위젯 안에서 이름이 "TimerText"인 텍스트 블록 찾기
            TimerTextBlock = Cast<UTextBlock>(GameHUDWidget->GetWidgetFromName(TEXT("TimerText")));
            // 위젯 안에서 이름이 "ZombieCountText"인 텍스트 블록 찾기
            ZombieCountTextBlock = Cast<UTextBlock>(GameHUDWidget->GetWidgetFromName(TEXT("ZombieCountText")));

            // 타이머 텍스트 블록이 없으면 경고 출력
            if (!TimerTextBlock)
                UE_LOG(LogTemp, Warning, TEXT("TimerTextBlock not found! Check widget hierarchy and name."));
            // 좀비 카운트 텍스트 블록이 없으면 경고 출력
            if (!ZombieCountTextBlock)
                UE_LOG(LogTemp, Warning, TEXT("ZombieCountTextBlock not found! Check widget hierarchy and name."));
        }
        else
        {
            // 위젯 생성 실패 시 경고
            UE_LOG(LogTemp, Warning, TEXT("Failed to create GameHUDWidget."));
        }
    }
    else
    {
        // GameHUDWidgetClass 자체가 에디터에서 지정되지 않은 경우
        UE_LOG(LogTemp, Warning, TEXT("GameHUDWidgetClass is not assigned in HUD."));
    }
}

// 남은 시간을 UI에 업데이트하는 함수
void ALighthouseHUD::UpdateTimerText(int32 RemainingTime)
{
    UE_LOG(LogTemp, Log, TEXT("UpdateTimerText called with RemainingTime: %d"), RemainingTime);

    if (TimerTextBlock) // 텍스트 블록이 존재하는 경우에만 실행
    {
        // 분, 초 계산
        int32 Minutes = RemainingTime / 60;
        int32 Seconds = RemainingTime % 60;
        // "Time Left: MM:SS" 형식의 문자열 생성
        FString TimeStr = FString::Printf(TEXT("Time Left: %02d:%02d"), Minutes, Seconds);
        // UI 텍스트 변경
        TimerTextBlock->SetText(FText::FromString(TimeStr));
    }
}

// 남은 좀비 수를 UI에 업데이트하는 함수
void ALighthouseHUD::UpdateZombieCount(int32 RemainingZombies)
{
    if (ZombieCountTextBlock) // 텍스트 블록이 존재하는 경우에만 실행
    {
        // "Zombies Left: N" 형식의 문자열 생성
        FString CountStr = FString::Printf(TEXT("Zombies Left: %d"), RemainingZombies);
        // UI 텍스트 변경
        ZombieCountTextBlock->SetText(FText::FromString(CountStr));
    }
}
