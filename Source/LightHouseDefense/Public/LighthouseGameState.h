#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LighthouseGameState.generated.h"

// === 브로드캐스트 델리게이트(기존 유지) ===
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAliveZombiesChanged, int32, AliveZombies);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillNormalChanged, int32, NewKillNormal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillTankChanged, int32, NewKillTank);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeUpdated, int32, RemainingTime);

UCLASS()
class ALighthouseGameState : public AGameStateBase
{
    GENERATED_BODY()
public:
    // === 타이머 제어(공개 API) ===
    UFUNCTION(BlueprintCallable) void StartCountdown();
    UFUNCTION(BlueprintCallable) void PauseCountdown();
    UFUNCTION(BlueprintCallable) void ResumeCountdown();

    // === Getter (기존 유지) ===
    UFUNCTION(BlueprintPure, Category = "HUD") int32 GetRemainingTime() const;
    UFUNCTION(BlueprintPure, Category = "HUD") int32 GetAliveZombiesTotal() const { return AliveZombiesTotal; }
    UFUNCTION(BlueprintPure, Category = "HUD") int32 GetKillNormal()        const { return KillNormal; }
    UFUNCTION(BlueprintPure, Category = "HUD") int32 GetKillTank()          const { return KillTank; }

    // === 상태 변경 API (방송 동반, 기존 유지) ===
    UFUNCTION(BlueprintCallable) void IncAliveZombiesTotal();
    UFUNCTION(BlueprintCallable) void DecAliveZombiesTotal();
    UFUNCTION(BlueprintCallable) void AddKillNormal();
    UFUNCTION(BlueprintCallable) void AddKillTank();

    // === UI가 구독하는 델리게이트(기존 유지) ===
    UPROPERTY(BlueprintAssignable) FOnTimeUpdated         OnTimeUpdated;
    UPROPERTY(BlueprintAssignable) FOnAliveZombiesChanged OnAliveZombiesChanged;
    UPROPERTY(BlueprintAssignable) FOnKillNormalChanged   OnKillNormalChanged;
    UPROPERTY(BlueprintAssignable) FOnKillTankChanged     OnKillTankChanged;

protected:
    virtual void BeginPlay() override;

private:
    void UpdateTimer();

    // === 타이머 상태 ===
    FTimerHandle CountdownTimerHandle;
    int32 RemainingTime = 600; // 기본 10분

    // === 좀비/킬 상태값(기존 유지) ===
    UPROPERTY(VisibleAnywhere, Category = "Zombies") int32 AliveZombiesTotal = 0;
    UPROPERTY(VisibleAnywhere, Category = "Zombies") int32 KillNormal = 0;
    UPROPERTY(VisibleAnywhere, Category = "Zombies") int32 KillTank = 0;
};
