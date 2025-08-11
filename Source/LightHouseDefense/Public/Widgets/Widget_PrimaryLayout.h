// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widget_PrimaryLayout.generated.h"

class UCommonActivatableWidgetContainerBase;

/**
 *
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick)) // 추상 클래스, 블루프린트에서 생성 가능, 기본 Tick 비활성화
class LIGHTHOUSEDEFENSE_API UWidget_PrimaryLayout : public UCommonUserWidget // Frontend UI의 메인 레이아웃 역할을 하는 위젯
{
    GENERATED_BODY()

public:
    UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& InTag) const;
    // 등록된 위젯 스택 맵에서 지정된 태그에 해당하는 스택을 찾아 반환

protected:
    UFUNCTION(BlueprintCallable) // 블루프린트에서 호출 가능
        void RegisterWidgetStack( // 새로운 위젯 스택을 태그와 함께 등록
            UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InStackTag, // 등록할 스택의 태그
            UCommonActivatableWidgetContainerBase* InStack // 등록할 위젯 스택 객체
        );

private:
    UPROPERTY(Transient) // 저장/직렬화되지 않는 임시 데이터
        TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> RegisteredWidgetStackMap;
    // 위젯 스택 태그와 해당 스택 객체를 매핑하여 저장하는 맵
};

