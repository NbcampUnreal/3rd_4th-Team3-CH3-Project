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

    void StartCountdown();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeUpdated, int32, RemainingTime);
    FOnTimeUpdated OnTimeUpdated;

    int32 GetRemainingTime() const;

private:
    void UpdateTimer();

    FTimerHandle CountdownTimerHandle;
    int32 RemainingTime = 300; // 5분
};
