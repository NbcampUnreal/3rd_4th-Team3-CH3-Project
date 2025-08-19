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
    // ADD: 터렛 해금 메시지 보여주기
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowUnlockText(int32 Index /*1~4*/, const FString& Message);

    // 무기 해금 전용 토스트
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowWeaponUnlockText(const FString& Message, float Duration = 3.f);

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UUserWidget> GameHUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> GameClearWidgetClass; // GameClear 위젯 BP 지정


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

    // ADD: Unlock 텍스트 캐시 + 자동숨김 타이머
    UPROPERTY() class UTextBlock* UnlockTextBlocks[4] = { nullptr, nullptr, nullptr, nullptr };
    FTimerHandle UnlockHideHandles[4];

    // WBP에 새로 만든 TextBlock
    UPROPERTY() class UTextBlock* WeaponUnlockText = nullptr;
    FTimerHandle TH_WeaponUnlockHide;

    bool bOneMinuteEventFired = false; // 60초 경고 1회만
    bool bGameClearShown = false;      // 클리어 UI 1회만

    // 60초 경고(색/깜빡임)
    FTimerHandle TH_MinuteBlink;
    bool bBlinkOn = false;
    FLinearColor NormalTimerColor = FLinearColor::White;
    FLinearColor WarnTimerColor = FLinearColor(1.f, 0.f, 0.f, 1.f);

    void ShowGameClearUI();
    void StartOneMinuteWarning();
    void StopOneMinuteWarning();
    void TickOneMinuteBlink();
};
