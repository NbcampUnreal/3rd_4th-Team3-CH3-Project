#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LighthouseHUD.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API ALighthouseHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION() // 반드시 있어야 OnTimeUpdated에 바인딩 가능
        void UpdateTimerText(int32 RemainingTime);

    UFUNCTION()
    void UpdateZombieCount(int32 RemainingZombies);

    UFUNCTION()
    void HandlePlayerHPChanged(float NewHP, float MaxHP);

    // ========================= [ADDED] =========================
    /** Kill(Normal) 값이 바뀔 때 UI에 반영 */
    UFUNCTION()
    void UpdateKillCountNormal(int32 NewCount);

    /** Kill(Tank) 값이 바뀔 때 UI에 반영 */
    UFUNCTION()
    void UpdateKillCountTank(int32 NewCount);
    // ==========================================================

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UUserWidget> GameHUDWidgetClass;

private:
    class UUserWidget* GameHUDWidget;
    class UTextBlock* TimerTextBlock;
    class UTextBlock* ZombieCountTextBlock;
    class UProgressBar* PlayerHP_ProgressBar = nullptr;

    // ========================= [ADDED] =========================
    // UMG 위젯 이름과 정확히 일치해야 함 (WBP_GameHUD)
    class UTextBlock* KillCountText_Normal = nullptr; // "KillCountText_Normal"
    class UTextBlock* KillCountText_Tank = nullptr; // "KillCountText_Tank"
    // ==========================================================
};
