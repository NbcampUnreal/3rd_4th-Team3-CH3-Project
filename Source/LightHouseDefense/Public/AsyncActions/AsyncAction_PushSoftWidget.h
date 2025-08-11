// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "AsyncAction_PushSoftWidget.generated.h"

class UWidget_ActivatableBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnPushSoftWidgetDelegate,       // 델리게이트 타입 이름
    UWidget_ActivatableBase*,        // 파라미터 타입
    PushedWidget                     // 파라미터 이름
);
// 블루프린트에서 바인딩 가능한 멀티캐스트 델리게이트 정의
// 위젯이 푸시될 때 해당 위젯을 인자로 이벤트 브로드캐스트 가능

/**
 * 비동기적으로 SoftClass로 된 UI 위젯을 로드하여
 * 지정된 위젯 스택에 푸시하는 Blueprint 액션 클래스
 */
UCLASS()
class LIGHTHOUSEDEFENSE_API UAsyncAction_PushSoftWidget
    : public UBlueprintAsyncActionBase // 블루프린트에서 비동기 호출을 지원하는 기본 클래스
{
    GENERATED_BODY()

public:
    // 블루프린트에서 호출 가능한 정적 함수
    // 월드 컨텍스트, 소유 PlayerController, 소프트 참조 위젯 클래스, 스택 태그, 포커스 여부를 입력받아 실행
    UFUNCTION(BlueprintCallable, meta = (
        WorldContext = "WorldContextObject",        // 월드 컨텍스트 지정
        HidePin = "WorldContextObject",             // 블루프린트에서 해당 핀 숨김
        BlueprintInternalUseOnly = "true",          // 내부 전용 (노드 검색 불가)
        DisplayName = "Push Soft Widget To Widget Stack" // 블루프린트 노드 표시 이름
        ))
    static UAsyncAction_PushSoftWidget* PushSoftWidget(
        const UObject* WorldContextObject,                            // 월드 컨텍스트
        APlayerController* OwningPlayerController,                    // 소유 PlayerController
        TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,      // 로드할 SoftClass UI 위젯
        UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InWidgetStackTag, // 스택 태그
        bool bFocusOnNewlyPushedWidget = true                          // 새로 푸시된 위젯에 포커스를 줄지 여부
    );

    //~ Begin UBlueprintAsyncActionBase Interface
    virtual void Activate() override; // 비동기 액션 시작 시 호출되는 함수
    //~ End UBlueprintAsyncActionBase Interface

    // 위젯 생성 직후(푸시 전) 호출되는 델리게이트
    UPROPERTY(BlueprintAssignable)
    FOnPushSoftWidgetDelegate OnWidgetCreatedBeforePush;

    // 위젯 푸시 완료 후 호출되는 델리게이트
    UPROPERTY(BlueprintAssignable)
    FOnPushSoftWidgetDelegate AfterPush;

private:
    // 캐싱된 실행 컨텍스트 및 입력값
    TWeakObjectPtr<UWorld> CachedOwningWorld;                       // 소유 월드
    TWeakObjectPtr<APlayerController> CachedOwningPC;               // 소유 PlayerController
    TSoftClassPtr<UWidget_ActivatableBase> CachedSoftWidgetClass;   // 로드할 SoftClass 위젯
    FGameplayTag CachedWidgetStackTag;                              // 대상 위젯 스택 태그
    bool bCachedFocusOnNewlyPushedWidget = false;                    // 포커스 여부 캐싱
};

