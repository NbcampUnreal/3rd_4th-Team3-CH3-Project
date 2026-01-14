#include "Widgets/GameClearWidget.h"
#include "LighthouseGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

// 생성: 텍스트 위젯 보정 → 현재 값 표시 → 델리게이트 구독
void UGameClearWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // BindWidget 실패(부모가 UserWidget 등) 시 대비해 이름으로 찾아서 보정
    ResolveTextBlocksByName();

    if (ALighthouseGameState* GS = GetWorld()->GetGameState<ALighthouseGameState>())
    {
        // 1) 현재 누적 킬 즉시 표시
        RefreshAll(GS->GetKillNormal(), GS->GetKillTank());

        // 2) 실시간 갱신(선택) - AddUniqueDynamic으로 중복 구독 방지
        if (!bBound)
        {
            GS->OnKillNormalChanged.AddUniqueDynamic(this, &UGameClearWidget::OnKillNormalChanged);
            GS->OnKillTankChanged.AddUniqueDynamic(this, &UGameClearWidget::OnKillTankChanged);
            bBound = true;
        }
    }
    else
    {
        // GameState가 아직 없다면 플레이스홀더
        RefreshAll(0, 0);
    }
}

// 파괴: 델리게이트 해제(PIE 재시작/여러 번 열기 대비)
void UGameClearWidget::NativeDestruct()
{
    if (bBound)
    {
        if (UWorld* W = GetWorld())
        {
            if (ALighthouseGameState* GS = W->GetGameState<ALighthouseGameState>())
            {
                GS->OnKillNormalChanged.RemoveDynamic(this, &UGameClearWidget::OnKillNormalChanged);
                GS->OnKillTankChanged.RemoveDynamic(this, &UGameClearWidget::OnKillTankChanged);
            }
        }
        bBound = false;
    }

    Super::NativeDestruct();
}

// 델리게이트 수신: 노말 값 변경 → 전체 갱신
void UGameClearWidget::OnKillNormalChanged(int32 NewCount)
{
    if (ALighthouseGameState* GS = GetWorld()->GetGameState<ALighthouseGameState>())
    {
        RefreshAll(NewCount, GS->GetKillTank());
    }
}

// 델리게이트 수신: 탱크 값 변경 → 전체 갱신
void UGameClearWidget::OnKillTankChanged(int32 NewCount)
{
    if (ALighthouseGameState* GS = GetWorld()->GetGameState<ALighthouseGameState>())
    {
        RefreshAll(GS->GetKillNormal(), NewCount);
    }
}

// 외부에서 스냅샷 값 주입이 필요할 때(대체 경로)
void UGameClearWidget::SetKills(int32 Normal, int32 Tank)
{
    RefreshAll(Normal, Tank);
}

// 한 번에 세 텍스트 갱신
void UGameClearWidget::RefreshAll(int32 Normal, int32 Tank)
{
    if (KillCountText_Normal)
    {
        KillCountText_Normal->SetText(
            FText::FromString(FString::Printf(TEXT("Kills (Normal): %d"), Normal)));
    }

    if (KillCountText_Tank)
    {
        KillCountText_Tank->SetText(
            FText::FromString(FString::Printf(TEXT("Kills (Tank): %d"), Tank)));
    }

    if (KillCountText_Total)
    {
        KillCountText_Total->SetText(
            FText::FromString(FString::Printf(TEXT("Total Kills: %d"), Normal + Tank)));
    }
}

// BindWidget 실패 시 이름으로 직접 찾아 포인터 보정
void UGameClearWidget::ResolveTextBlocksByName()
{
    // 이미 바인딩되어 있으면 패스
    if (KillCountText_Normal && KillCountText_Tank && KillCountText_Total) return;

    // 위젯 트리에서 동일한 이름으로 탐색(BP의 Is Variable 필요 없음)
    if (!KillCountText_Normal)
    {
        if (UWidget* W = GetWidgetFromName(TEXT("KillCountText_Normal")))
        {
            KillCountText_Normal = Cast<UTextBlock>(W);
        }
    }
    if (!KillCountText_Tank)
    {
        if (UWidget* W = GetWidgetFromName(TEXT("KillCountText_Tank")))
        {
            KillCountText_Tank = Cast<UTextBlock>(W);
        }
    }
    if (!KillCountText_Total)
    {
        if (UWidget* W = GetWidgetFromName(TEXT("KillCountText_Total")))
        {
            KillCountText_Total = Cast<UTextBlock>(W);
        }
    }
}
