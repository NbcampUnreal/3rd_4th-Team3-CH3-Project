#include "LighthouseGameState.h"
#include "Kismet/GameplayStatics.h"
#include "LighthouseHUD.h"
#include "Engine/World.h"
#include "TimerManager.h"

void ALighthouseGameState::BeginPlay()
{
    Super::BeginPlay();
    StartCountdown();

    // 첫 번째 플레이어 컨트롤러를 가져옴 (싱글플레이어 기준)
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        // 해당 컨트롤러에서 HUD를 가져와서 커스텀 HUD 타입으로 캐스팅
        ALighthouseHUD* LH_HUD = Cast<ALighthouseHUD>(PC->GetHUD());
        if (LH_HUD)
        {
            // HUD에 타이머 업데이트 함수 바인딩
           // OnTimeUpdated가 호출될 때마다 HUD의 UpdateTimerText 함수가 실행됨
            OnTimeUpdated.AddDynamic(LH_HUD, &ALighthouseHUD::UpdateTimerText);

        }
    }
}

// 카운트다운을 시작
void ALighthouseGameState::StartCountdown()
{
    // 1초마다 UpdateTimer() 함수를 반복적으로 호출하도록 타이머 설정
    GetWorldTimerManager().SetTimer(
        CountdownTimerHandle, // 타이머 핸들
        this, // 대상 객체
        &ALighthouseGameState::UpdateTimer, // 호출할 함수
        1.0f, // 호출 간격 (초 단위)
        true // 반복 호출 여부 (true: 반복)
    );
}
// 1초마다 호출되는 함수: 타이머 값을 감소시키고 UI에 알림
void ALighthouseGameState::UpdateTimer()
{
    RemainingTime--;// 남은 시간 1초 감소

    if (RemainingTime <= 0)
    {
        RemainingTime = 0; // 0 미만으로 내려가지 않도록 고정

        GetWorldTimerManager().ClearTimer(CountdownTimerHandle); // 타이머 중지
    }
    // 델리게이트를 통해 남은 시간 전달 → HUD 등에서 UI 갱신 가능
    OnTimeUpdated.Broadcast(RemainingTime);
}
// 현재 남은 시간을 반환하는 Getter 함수
int32 ALighthouseGameState::GetRemainingTime() const
{
    return RemainingTime;
}


void ALighthouseGameState::PauseCountdown()
{
    GetWorldTimerManager().PauseTimer(CountdownTimerHandle);
}

void ALighthouseGameState::ResumeCountdown()
{
    GetWorldTimerManager().UnPauseTimer(CountdownTimerHandle);
}
