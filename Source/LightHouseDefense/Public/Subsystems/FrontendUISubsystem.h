// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendUISubsystem.generated.h"

class UWidget_PrimaryLayout;
struct FGameplayTag;
class UWidget_ActivatableBase;
class UFrontendCommonButtonBase;

enum class EAsyncPushWidgetState : uint8 // 비동기 위젯 푸시 상태를 나타내는 열거형 (1바이트 크기)
{
    OnCreatedBeforePush, // 위젯이 생성되었지만 아직 스택에 푸시되기 전 상태
    AfterPush             // 위젯이 스택에 푸시된 직후 상태
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate,
    UFrontendCommonButtonBase*,
    BroadcastingButton,
    FText,
    DescriptionText);
/**
 * - Frontend UI를 관리하는 GameInstance 서브시스템
 * - Primary Layout 위젯 등록, SoftClass UI 위젯의 비동기 로드 및 스택 푸시 기능 제공
 */
UCLASS()
class LIGHTHOUSEDEFENSE_API UFrontendUISubsystem : public UGameInstanceSubsystem // 게임 전체에서 공유되는 UI 서브시스템
{
    GENERATED_BODY() // 언리얼 리플렉션 및 기본 생성 코드 자동 생성

public:
    static UFrontendUISubsystem* Get(const UObject* WorldContextObject); // WorldContext에서 FrontendUISubsystem 인스턴스 가져오기

    //~ Begin USubsystem Interface
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override; // 서브시스템 생성 여부 결정 (예: 전용 서버에서는 생성 안 함)
    //~ End USubsystem Interface

    UFUNCTION(BlueprintCallable) // 블루프린트에서 호출 가능
        void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget); // 생성된 Primary Layout 위젯 등록

    void PushSoftWidgetToStackAsync( // SoftClass UI 위젯을 비동기로 로드 후 지정 스택에 푸시
        const FGameplayTag& InWidgetStackTag, // 대상 스택 태그
        TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, // 로드할 SoftClass UI 위젯
        TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AysncPushStateCallback // 상태 변화 시 호출될 콜백
    );

    UPROPERTY(BlueprintAssignable)
    FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdated;

private:
    UPROPERTY(Transient) // 저장/직렬화되지 않는 임시 값
        UWidget_PrimaryLayout* CreatedPrimaryLayout; // 등록된 Primary Layout 위젯 (UI 스택 접근 시 사용)
};

