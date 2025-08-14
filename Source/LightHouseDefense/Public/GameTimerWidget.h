#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameTimerWidget.generated.h"


class UTextBlock;


UCLASS()
class UGameTimerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void UpdateTimeText(int32 RemainingSeconds);

    // [ADDED] PIE 종료/재시작 시 바인딩 해제용
    virtual void NativeDestruct() override; // [ADDED]

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimerText;

    // [ADDED] 게임 클리어(혹은 게임 오버) 위젯 클래스를 에디터에서 지정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> GameClearWidgetClass; // [ADDED]

private:
    // [ADDED] 한 번만 띄우기 위한 가드
    bool bGameClearShown = false; // [ADDED]

    // [ADDED] 위젯 생성 + 일시정지 + 입력 모드 전환
    void ShowGameClearUI(); // [ADDED]
};
