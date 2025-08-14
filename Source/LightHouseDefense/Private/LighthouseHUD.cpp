#include "LighthouseHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ProgressBar.h"         // ProgressBar 접근
#include "Kismet/GameplayStatics.h"         // GetPlayerPawn
#include "GameFramework/PlayerController.h" // GetOwningPlayerController
#include "HealthComponent.h"                // UHealthComponent
#include "LighthouseGameState.h"            // [ADDED]
#include "LightHouseCharacter.h" 
#include "EngineUtils.h"

// 게임 시작 시 HUD 초기화
void ALighthouseHUD::BeginPlay()
{
    Super::BeginPlay();

    // HUD 위젯 클래스가 지정되어 있는지 확인
    if (GameHUDWidgetClass)
    {
        UE_LOG(LogTemp, Log, TEXT("Creating GameHUDWidget."));
        // 지정된 UUserWidget 클래스로 HUD 생성
        GameHUDWidget = CreateWidget<UUserWidget>(GetWorld(), GameHUDWidgetClass);
        if (GameHUDWidget)
        {
            // 화면에 위젯 표시
            GameHUDWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("GameHUDWidget added to viewport."));
            // 위젯 안에서 이름이 "TimerText"인 텍스트 블록 찾기
            TimerTextBlock = Cast<UTextBlock>(GameHUDWidget->GetWidgetFromName(TEXT("TimerText")));
            // 위젯 안에서 이름이 "ZombieCountText"인 텍스트 블록 찾기
            ZombieCountTextBlock = Cast<UTextBlock>(GameHUDWidget->GetWidgetFromName(TEXT("ZombieCountText")));

            // 타이머 텍스트 블록이 없으면 경고 출력
            if (!TimerTextBlock)
                UE_LOG(LogTemp, Warning, TEXT("TimerTextBlock not found! Check widget hierarchy and name."));
            // 좀비 카운트 텍스트 블록이 없으면 경고 출력
            if (!ZombieCountTextBlock)
                UE_LOG(LogTemp, Warning, TEXT("ZombieCountTextBlock not found! Check widget hierarchy and name."));

            // 플레이어 HP 위젯 찾기 (위젯 이름 정확히: PlayerHP )
            PlayerHP_ProgressBar = Cast<UProgressBar>(GameHUDWidget->GetWidgetFromName(TEXT("PlayerHP")));
            if (!PlayerHP_ProgressBar)
                UE_LOG(LogTemp, Warning, TEXT("PlayerHP not found! (WBP_GameHUD)"));

            // 등대 HP 위젯 찾기 (위젯 이름 정확히: LighthouseHP )
            LighthouseHP_ProgressBar = Cast<UProgressBar>(GameHUDWidget->GetWidgetFromName(TEXT("LighthouseHP")));
            if (!LighthouseHP_ProgressBar)
                UE_LOG(LogTemp, Warning, TEXT("LighthouseHP not found! (WBP_GameHUD)"));

            // ========================= [ADDED] =========================
            // 킬 카운트 텍스트 (정확한 이름 필수)
            KillCountText_Normal = Cast<UTextBlock>(GameHUDWidget->GetWidgetFromName(TEXT("KillCountText_Normal")));
            KillCountText_Tank = Cast<UTextBlock>(GameHUDWidget->GetWidgetFromName(TEXT("KillCountText_Tank")));
            if (!KillCountText_Normal)
                UE_LOG(LogTemp, Warning, TEXT("KillCountText_Normal not found! (WBP_GameHUD)"));
            if (!KillCountText_Tank)
                UE_LOG(LogTemp, Warning, TEXT("KillCountText_Tank not found! (WBP_GameHUD)"));
            // ==========================================================

        }
        else
        {
            // 위젯 생성 실패 시 경고
            UE_LOG(LogTemp, Warning, TEXT("Failed to create GameHUDWidget."));
        }
    }
    else
    {
        // GameHUDWidgetClass 자체가 에디터에서 지정되지 않은 경우
        UE_LOG(LogTemp, Warning, TEXT("GameHUDWidgetClass is not assigned in HUD."));
    }

    // 로컬 플레이어 Pawn의 HealthComponent 찾아서 델리게이트 바인딩
    APawn* PlayerPawn =
        (GetOwningPlayerController() && GetOwningPlayerController()->GetPawn())
        ? GetOwningPlayerController()->GetPawn()
        : UGameplayStatics::GetPlayerPawn(this, 0);

    if (PlayerPawn)
    {
        if (UHealthComponent* HC = PlayerPawn->FindComponentByClass<UHealthComponent>())
        {
            // 초기값 1회 즉시 반영
            HandlePlayerHPChanged(HC->GetHealth(), HC->GetMaxHealth());

            // 체력 변경 시마다 호출되도록 바인딩
            HC->OnHealthChanged.AddDynamic(this, &ALighthouseHUD::HandlePlayerHPChanged);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Player Pawn has no HealthComponent."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerPawn not found on BeginPlay."));
    }

    //등대 hp 바인딩
    {
        ALightHouseCharacter* FoundLighthouse = nullptr;

        // 월드에 여러 개 있을 수 있으니 우선 첫 번째만 사용(필요시 더 정교한 선택 로직으로 교체)
        for (TActorIterator<ALightHouseCharacter> It(GetWorld()); It; ++It)
        {
            FoundLighthouse = *It;
            break;
        }

        if (FoundLighthouse)
        {
            if (UHealthComponent* LH_HC = FoundLighthouse->FindComponentByClass<UHealthComponent>())
            {
                // 초기값 즉시 반영
                HandleLighthouseHPChanged(LH_HC->GetHealth(), LH_HC->GetMaxHealth());

                // 중복 바인딩 방지 후 구독
                if (!LH_HC->OnHealthChanged.IsAlreadyBound(this, &ALighthouseHUD::HandleLighthouseHPChanged))
                {
                    LH_HC->OnHealthChanged.AddDynamic(this, &ALighthouseHUD::HandleLighthouseHPChanged);
                }

                UE_LOG(LogTemp, Log, TEXT("[HUD] Bound to Lighthouse HealthComponent: %s"), *FoundLighthouse->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Found Lighthouse but no HealthComponent on it."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No ALightHouseCharacter found in world. (Place the Lighthouse actor in map)"));
        }
    }

    // HUD가 직접 GameState 델리게이트에 바인딩 (타이머 + 좀비 카운트 + 킬 분리)
    if (ALighthouseGameState* GS = GetWorld()->GetGameState<ALighthouseGameState>())
    {
        GS->OnTimeUpdated.AddDynamic(this, &ALighthouseHUD::UpdateTimerText);
        GS->OnAliveZombiesChanged.AddDynamic(this, &ALighthouseHUD::UpdateZombieCount);
        GS->OnKillNormalChanged.AddDynamic(this, &ALighthouseHUD::UpdateKillCountNormal);
        GS->OnKillTankChanged.AddDynamic(this, &ALighthouseHUD::UpdateKillCountTank);

        // 시작 시 1회 초기 반영
        UpdateTimerText(GS->GetRemainingTime());
        UpdateZombieCount(GS->GetAliveZombiesTotal());
        UpdateKillCountNormal(GS->GetKillNormal());
        UpdateKillCountTank(GS->GetKillTank());

        UE_LOG(LogTemp, Log, TEXT("[HUD] Bound to GameState delegates (time/alive/kill-normal/kill-tank)"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[HUD] GameState not found; bindings skipped"));
    }
}

// 남은 시간을 UI에 업데이트하는 함수
void ALighthouseHUD::UpdateTimerText(int32 RemainingTime)
{
    UE_LOG(LogTemp, Log, TEXT("UpdateTimerText called with RemainingTime: %d"), RemainingTime);

    if (TimerTextBlock) // 텍스트 블록이 존재하는 경우에만 실행
    {
        // 분, 초 계산
        int32 Minutes = RemainingTime / 60;
        int32 Seconds = RemainingTime % 60;
        // "Time Left: MM:SS" 형식의 문자열 생성
        FString TimeStr = FString::Printf(TEXT("Time: %02d:%02d"), Minutes, Seconds);
        // UI 텍스트 변경
        TimerTextBlock->SetText(FText::FromString(TimeStr));
    }
}

// 남은 좀비 수를 UI에 업데이트하는 함수(Alive 총합 표시)
void ALighthouseHUD::UpdateZombieCount(int32 RemainingZombies)
{
    if (ZombieCountTextBlock) // 텍스트 블록이 존재하는 경우에만 실행
    {
        // "Zombies Left: N" 형식의 문자열 생성
        FString CountStr = FString::Printf(TEXT("Zombies: %d"), RemainingZombies);
        // UI 텍스트 변경
        ZombieCountTextBlock->SetText(FText::FromString(CountStr));
    }
}

void ALighthouseHUD::HandlePlayerHPChanged(float NewHP, float MaxHP)
{
    // 게이지
    if (PlayerHP_ProgressBar)
    {
        const float Pct = (MaxHP > 0.f) ? (NewHP / MaxHP) : 0.f;
        PlayerHP_ProgressBar->SetPercent(Pct);
    }

    UE_LOG(LogTemp, Log, TEXT("[HUD] HP changed: %.0f / %.0f"), NewHP, MaxHP);
}

// FIX: 등대 HP 반영
void ALighthouseHUD::HandleLighthouseHPChanged(float NewHP, float MaxHP)
{
    if (LighthouseHP_ProgressBar)
    {
        const float Pct = (MaxHP > 0.f) ? (NewHP / MaxHP) : 0.f;
        LighthouseHP_ProgressBar->SetPercent(Pct);
    }
    UE_LOG(LogTemp, Log, TEXT("[HUD] Lighthouse HP: %.0f / %.0f"), NewHP, MaxHP);
}

// ========================= [ADDED] =========================
// 일반 킬 수 UI 반영: "KillCountText_Normal"에 표시
void ALighthouseHUD::UpdateKillCountNormal(int32 NewCount)
{
    if (KillCountText_Normal)
    {
        KillCountText_Normal->SetText(FText::FromString(
            FString::Printf(TEXT("Kills (Normal): %d"), NewCount)));
    }
}

// 탱크 킬 수 UI 반영: "KillCountText_Tank"에 표시
void ALighthouseHUD::UpdateKillCountTank(int32 NewCount)
{
    if (KillCountText_Tank)
    {
        KillCountText_Tank->SetText(FText::FromString(
            FString::Printf(TEXT("Kills (Tank): %d"), NewCount)));
    }
}
// ==========================================================
