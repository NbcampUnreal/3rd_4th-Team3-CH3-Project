// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Widget_ActivatableBase.h"

// 비동기 액션으로 소프트 위젯을 특정 스택에 푸시하는 함수
UAsyncAction_PushSoftWidget* UAsyncAction_PushSoftWidget::PushSoftWidget(
    const UObject* WorldContextObject,                           // 월드 컨텍스트
    APlayerController* OwningPlayerController,                   // 위젯의 소유 PlayerController
    TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,     // 로드할 소프트 위젯 클래스
    UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InWidgetStackTag, // 푸시할 위젯 스택 태그
    bool bFocusOnNewlyPushedWidget                                // 푸시 후 새 위젯에 포커스를 줄지 여부
)
{
    // 위젯 클래스가 비어있으면 에러
    checkf(!InSoftWidgetClass.IsNull(), TEXT("PushSoftWidgetToStack was passed a null soft widget class "));

    if (GEngine)
    {
        // 월드 가져오기
        if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
        {
            // 액션 객체 생성 및 데이터 캐싱
            UAsyncAction_PushSoftWidget* Node = NewObject<UAsyncAction_PushSoftWidget>();
            Node->CachedOwningWorld = World;
            Node->CachedOwningPC = OwningPlayerController;
            Node->CachedSoftWidgetClass = InSoftWidgetClass;
            Node->CachedWidgetStackTag = InWidgetStackTag;
            Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;

            // 게임 인스턴스에 등록
            Node->RegisterWithGameInstance(World);

            return Node;
        }
    }

    return nullptr;
}

// 액션 실행 (위젯 비동기 푸시 로직)
void UAsyncAction_PushSoftWidget::Activate()
{
    // 프론트엔드 UI 서브시스템 가져오기
    UFrontendUISubsystem* FrontendUISubsystem = UFrontendUISubsystem::Get(CachedOwningWorld.Get());

    // 위젯을 비동기로 스택에 푸시
    FrontendUISubsystem->PushSoftWidgetToStackAsync(
        CachedWidgetStackTag,
        CachedSoftWidgetClass,
        [this](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget) // 콜백
        {
            switch (InPushState)
            {
            case EAsyncPushWidgetState::OnCreatedBeforePush: // 위젯 생성 직후
                PushedWidget->SetOwningPlayer(CachedOwningPC.Get());
                OnWidgetCreatedBeforePush.Broadcast(PushedWidget); // 생성 이벤트 브로드캐스트
                break;

            case EAsyncPushWidgetState::AfterPush: // 스택에 푸시된 이후
                AfterPush.Broadcast(PushedWidget); // 푸시 완료 이벤트 브로드캐스트

                // 포커스 설정
                if (bCachedFocusOnNewlyPushedWidget)
                {
                    if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
                    {
                        WidgetToFocus->SetFocus();
                    }
                }

                SetReadyToDestroy(); // 액션 객체 소멸 준비
                break;

            default:
                break;
            }
        }
    );
}

