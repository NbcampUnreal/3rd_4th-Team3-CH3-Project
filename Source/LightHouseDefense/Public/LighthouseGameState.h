#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LighthouseGameState.generated.h"

UCLASS()
class ALighthouseGameState : public AGameStateBase
{
    GENERATED_BODY()
public:

    virtual void BeginPlay() override;
    // 카운트다운 타이머를 시작
    void StartCountdown();

    // ⬇⬇ 추가 (게임 일시정지/재개용)
    UFUNCTION(BlueprintCallable) void PauseCountdown();
    UFUNCTION(BlueprintCallable) void ResumeCountdown();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeUpdated, int32, RemainingTime);

    // 델리게이트 인스턴스. 다른 클래스(예: HUD, 위젯 등)에서 여기에 바인딩하여 타이머 UI 업데이트 가능
    FOnTimeUpdated OnTimeUpdated;

    // 현재 남은 시간을 반환하는 함수 (외부에서 남은 시간 읽을 때 사용)
    int32 GetRemainingTime() const;



private:
    // 일정 주기마다 호출되어 타이머를 갱신
    void UpdateTimer();

    // 카운트다운에 사용할 타이머 핸들 (타이머 관리에 필요)
    FTimerHandle CountdownTimerHandle;
    // 남은 시간(초 단위) — 기본값은 600초 (10분)
    int32 RemainingTime = 600;
};
