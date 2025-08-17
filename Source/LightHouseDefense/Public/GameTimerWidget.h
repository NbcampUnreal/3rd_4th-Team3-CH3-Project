#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameTimerWidget.generated.h"

// [NEW] 블루프린트에서 바인드할 “1분 남음” 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOneMinuteLeft);

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

    // [NEW] BP에서 바인드할 수 있는 이벤트: 60초 이하로 처음 진입했을 때 1회만 호출
    UPROPERTY(BlueprintAssignable, Category = "Timer")
    FOnOneMinuteLeft OnOneMinuteLeft; // [NEW]

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimerText;

    // [ADDED] 게임 클리어(혹은 게임 오버) 위젯 클래스를 에디터에서 지정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> GameClearWidgetClass; // [ADDED]

private:
    // [ADDED] 한 번만 띄우기 위한 가드
    bool bGameClearShown = false; // [ADDED]

    // [NEW] 1분 이벤트를 중복 발사하지 않기 위한 가드
    bool bOneMinuteEventFired = false; // [NEW]

    // [NEW] 최근 남은 시간을 캐싱(원하면 BP에서 읽어 쓸 수 있게 확장 가능)
    int32 CachedRemainingSeconds = 0; // [NEW]

    // [ADDED] 위젯 생성 + 일시정지 + 입력 모드 전환
    void ShowGameClearUI(); // [ADDED]
};
