// === LighthouseGameState.cpp ===
#include "LighthouseGameState.h"
#include "TimerManager.h"

void ALighthouseGameState::BeginPlay()
{
    Super::BeginPlay();

    // FIX: 자동 카운트다운 시작을 비활성화(주석 처리).
    //      F키 트리거가 GameMode::StartRun()에서 StartCountdown()을 호출하도록 설계.
    // StartCountdown(); // <-- 필요 시 자동 시작으로 되돌리려면 주석 해제

    // NOTE: 초기값 1회 방송은 유지(위젯이 붙어도 바로 보이도록)
    OnTimeUpdated.Broadcast(RemainingTime);
    OnAliveZombiesChanged.Broadcast(AliveZombiesTotal);
    OnKillNormalChanged.Broadcast(KillNormal);
    OnKillTankChanged.Broadcast(KillTank);
}

void ALighthouseGameState::StartCountdown()
{
    // 기존 유지: 1초 타이머 시작
    GetWorldTimerManager().SetTimer(
        CountdownTimerHandle,
        this,
        &ALighthouseGameState::UpdateTimer,
        1.0f,
        true
    );
}

void ALighthouseGameState::PauseCountdown()
{
    GetWorldTimerManager().PauseTimer(CountdownTimerHandle);
}

void ALighthouseGameState::ResumeCountdown()
{
    GetWorldTimerManager().UnPauseTimer(CountdownTimerHandle);
}

void ALighthouseGameState::UpdateTimer()
{
    RemainingTime = FMath::Max(0, RemainingTime - 1);
    OnTimeUpdated.Broadcast(RemainingTime);
    if (RemainingTime <= 0)
    {
        GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
    }
}

// === Getter (시그니처 일치로 LNK 방지) ===
int32 ALighthouseGameState::GetRemainingTime() const
{
    return RemainingTime;
}

// === 상태 변경 함수들(방송 동반, 기존 유지) ===
void ALighthouseGameState::IncAliveZombiesTotal()
{
    ++AliveZombiesTotal;
    OnAliveZombiesChanged.Broadcast(AliveZombiesTotal); // FIX: UI 갱신
}

void ALighthouseGameState::DecAliveZombiesTotal()
{
    AliveZombiesTotal = FMath::Max(0, AliveZombiesTotal - 1);
    OnAliveZombiesChanged.Broadcast(AliveZombiesTotal); // FIX
}

void ALighthouseGameState::AddKillNormal()
{
    ++KillNormal;
    OnKillNormalChanged.Broadcast(KillNormal);          // FIX
}

void ALighthouseGameState::AddKillTank()
{
    ++KillTank;
    OnKillTankChanged.Broadcast(KillTank);              // FIX
}
