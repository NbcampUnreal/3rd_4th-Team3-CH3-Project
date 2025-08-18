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

    // [ADD] 무기 없음(맨손)일 때 탄약 텍스트를 "- / -"로 표시
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetAmmoPlaceholder();

    // 무기로 다시 전환했을 때 HUD가 현재 무기와 즉시 재바인딩
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void RebindAmmoToCurrentWeapon();

    UFUNCTION() // 반드시 있어야 OnTimeUpdated에 바인딩 가능
        void UpdateTimerText(int32 RemainingTime);

    UFUNCTION()
    void UpdateZombieCount(int32 RemainingZombies);

    UFUNCTION()
    void HandlePlayerHPChanged(float NewHP, float MaxHP);

    UFUNCTION()
    void HandleLighthouseHPChanged(float NewHP, float MaxHP); //등대hp

    // ========================= [ADDED] =========================
    /** Kill(Normal) 값이 바뀔 때 UI에 반영 */
    UFUNCTION()
    void UpdateKillCountNormal(int32 NewCount);

    /** Kill(Tank) 값이 바뀔 때 UI에 반영 */
    UFUNCTION()
    void UpdateKillCountTank(int32 NewCount);
    // ==========================================================

    // ========================= [ADD] AmmoText ⇄ Weapon 바인딩 =========================
    /** 무기를 찾아 AmmoText(TextBlock)를 연결한다. 무기가 아직 없으면 타이머로 재시도 */
    UFUNCTION()
    void TryBindWeaponToAmmoText(); // [ADD]
    // ================================================================================

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UUserWidget> GameHUDWidgetClass;

private:
    class UUserWidget* GameHUDWidget = nullptr;
    class UTextBlock* TimerTextBlock = nullptr;
    class UTextBlock* ZombieCountTextBlock = nullptr;
    class UProgressBar* PlayerHP_ProgressBar = nullptr;
    class UProgressBar* LighthouseHP_ProgressBar = nullptr;

    // ========================= [ADDED] =========================
    // UMG 위젯 이름과 정확히 일치해야 함 (WBP_GameHUD)
    class UTextBlock* KillCountText_Normal = nullptr; // "KillCountText_Normal"
    class UTextBlock* KillCountText_Tank = nullptr;   // "KillCountText_Tank"
    // ==========================================================

    // ========================= [ADD] AmmoText 포인터 & 타이머 =========================
    class UTextBlock* AmmoTextBlock = nullptr;        // "AmmoText" 라는 이름의 TextBlock
    FTimerHandle TH_TryBindWeapon;                    // 무기 생길 때까지 재시도
    // ================================================================================
    TWeakObjectPtr<class AWeapon> BoundWeapon;  // [ADD] 현재 AmmoTextBlock에 연결돼있는 무기
};
