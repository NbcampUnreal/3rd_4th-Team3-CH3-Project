#include "LighthouseHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ProgressBar.h"         // ProgressBar 접근
#include "Kismet/GameplayStatics.h"         // GetPlayerPawn
#include "GameFramework/PlayerController.h" // GetOwningPlayerController
#include "HealthComponent.h"                // UHealthComponent
#include "LighthouseGameState.h"

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

          // HUD가 직접 GameState 델리게이트에 바인딩 (타이밍 문제 해결)
    if (ALighthouseGameState* GS = GetWorld()->GetGameState<ALighthouseGameState>())
    {
        GS->OnTimeUpdated.AddDynamic(this, &ALighthouseHUD::UpdateTimerText);
        // 시작 시 1회 초기 반영
        UpdateTimerText(GS->GetRemainingTime());
        UE_LOG(LogTemp, Log, TEXT("[HUD] Bound to OnTimeUpdated on GameState"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[HUD] GameState not found; timer binding skipped"));
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
        FString TimeStr = FString::Printf(TEXT("Time Left: %02d:%02d"), Minutes, Seconds);
        // UI 텍스트 변경
        TimerTextBlock->SetText(FText::FromString(TimeStr));
    }
}

// 남은 좀비 수를 UI에 업데이트하는 함수
void ALighthouseHUD::UpdateZombieCount(int32 RemainingZombies)
{
    if (ZombieCountTextBlock) // 텍스트 블록이 존재하는 경우에만 실행
    {
        // "Zombies Left: N" 형식의 문자열 생성
        FString CountStr = FString::Printf(TEXT("Zombies Left: %d"), RemainingZombies);
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
