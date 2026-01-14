#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameClearWidget.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API UGameClearWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // (선택) 외부에서 스냅샷 값 주입이 필요할 때 호출
    UFUNCTION(BlueprintCallable, Category = "GameClear")
    void SetKills(int32 Normal, int32 Tank);

protected:
    // 생성: 텍스트 위젯 찾기 → 초기값 반영 → 델리게이트 구독
    virtual void NativeConstruct() override;
    // 파괴: 델리게이트 언바인드
    virtual void NativeDestruct() override;

    // 자동 바인딩(부모 클래스를 C++로 설정했다면 BP 이름과 매칭)
    UPROPERTY(meta = (BindWidget)) UTextBlock* KillCountText_Normal = nullptr; // "Kills (Normal): N"
    UPROPERTY(meta = (BindWidget)) UTextBlock* KillCountText_Tank = nullptr; // "Kills (Tank): N"
    UPROPERTY(meta = (BindWidget)) UTextBlock* KillCountText_Total = nullptr; // "Total Kills: N"

    // GameState 델리게이트 수신
    UFUNCTION() void OnKillNormalChanged(int32 NewCount);
    UFUNCTION() void OnKillTankChanged(int32 NewCount);

private:
    // 화면 갱신
    void RefreshAll(int32 Normal, int32 Tank);
    // 위젯 포인터 보정(BindWidget 실패 대비)
    void ResolveTextBlocksByName();

    bool bBound = false; // 델리게이트 구독 여부
};
