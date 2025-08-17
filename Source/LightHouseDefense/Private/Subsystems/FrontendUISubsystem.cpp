// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/FrontendUISubsystem.h"
#include "Engine/AssetManager.h"
#include "Widgets/Widget_PrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widgets/Widget_ConfirmScreen.h"
#include "FrontendGameplayTags.h"
#include "FrontendFunctionLibrary.h"


// Frontend UI Subsystem 클래스 함수 구현부
// UI 위젯 관리, 비동기 로드 및 스택 푸시 기능을 제공
UFrontendUISubsystem* UFrontendUISubsystem::Get(const UObject* WorldContextObject)
{
    if (GEngine)
    {
        // WorldContextObject로부터 UWorld 가져오기
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

        // 현재 GameInstance에서 FrontendUISubsystem 가져오기
        return UGameInstance::GetSubsystem<UFrontendUISubsystem>(World->GetGameInstance());
    }

    return nullptr; // GEngine이 없을 경우
}

bool UFrontendUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    // 전용 서버(디디케이티드 서버) 환경에서는 서브시스템 생성 X
    if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
    {
        TArray<UClass*> FoundClasses;
        // 현재 클래스의 모든 파생 클래스 검색
        GetDerivedClasses(GetClass(), FoundClasses);

        // 파생 클래스가 없을 때만 서브시스템 생성
        return FoundClasses.IsEmpty();
    }

    return false;
}

void UFrontendUISubsystem::RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget)
{
    check(InCreatedWidget); // nullptr 방지

    // 생성된 Primary Layout 위젯 저장
    CreatedPrimaryLayout = InCreatedWidget;

    //Debug::Print(TEXT("Primary layout widget stored")); // 디버그 출력 (현재 주석 처리됨)
}

void UFrontendUISubsystem::PushSoftWidgetToStackAsync(
    const FGameplayTag& InWidgetStackTag,                        // 푸시할 스택 태그
    TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,     // 로드할 소프트 위젯 클래스
    TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AysncPushStateCallback // 상태 콜백
)
{
    check(!InSoftWidgetClass.IsNull()); // 위젯 클래스가 비어있는지 확인

    // 비동기 로드를 요청 (AssetManager 사용)
    UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
        InSoftWidgetClass.ToSoftObjectPath(), // 로드 경로
        FStreamableDelegate::CreateLambda(    // 로드 완료 후 실행될 람다
            [InSoftWidgetClass, this, InWidgetStackTag, AysncPushStateCallback]()
            {
                // 로드된 클래스 가져오기
                UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
                check(LoadedWidgetClass && CreatedPrimaryLayout); // 필수 데이터 확인

                // 해당 태그의 위젯 스택 찾기
                UCommonActivatableWidgetContainerBase* FoundWidgetStack =
                    CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);

                // 위젯 생성 (스택에 추가)
                UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(
                    LoadedWidgetClass,
                    [AysncPushStateCallback](UWidget_ActivatableBase& CreatedWidgetInstance)
                    {
                        // 생성 직후 상태 콜백 호출
                        AysncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
                    }
                );

                // 푸시 완료 후 상태 콜백 호출
                AysncPushStateCallback(EAsyncPushWidgetState::AfterPush, CreatedWidget);
            }
        )
    );
}

void UFrontendUISubsystem::PushConfirmScreenToModalStackAynsc(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMsg, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback)
{
    UConfirmScreenInfoObject* CreatedInfoObject = nullptr;

    switch (InScreenType)
    {
    case EConfirmScreenType::Ok:
        CreatedInfoObject = UConfirmScreenInfoObject::CreateOKScreen(InScreenTitle, InScreenMsg);
        break;

    case EConfirmScreenType::YesNo:
        CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMsg);
        break;

    case EConfirmScreenType::OKCancel:
        CreatedInfoObject = UConfirmScreenInfoObject::CreateOkCancelScreen(InScreenTitle, InScreenMsg);
        break;

    case EConfirmScreenType::Unknown:
        break;
    default:
        break;
    }

    check(CreatedInfoObject);

    PushSoftWidgetToStackAsync(
        FrontendGameplayTags::Frontend_WidgetStack_Modal,
        UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_ConfirmScreen),
        [CreatedInfoObject, ButtonClickedCallback](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
        {
            if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
            {
                UWidget_ConfirmScreen* CreatedConfirmScreen = CastChecked<UWidget_ConfirmScreen>(PushedWidget);
                CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
            }
        }
    );
}
