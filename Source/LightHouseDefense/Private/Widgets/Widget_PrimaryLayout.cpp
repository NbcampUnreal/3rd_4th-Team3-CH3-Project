// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Widget_PrimaryLayout.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
    // 전달받은 태그가 위젯 스택 맵에 존재하는지 확인 (없으면 에러)
    checkf(
        RegisteredWidgetStackMap.Contains(InTag),
        TEXT("Can not find the widget stack by the tag %s"),
        *InTag.ToString()
    );

    // 해당 태그에 매핑된 위젯 스택 반환
    return RegisteredWidgetStackMap.FindRef(InTag);
}

void UWidget_PrimaryLayout::RegisterWidgetStack(
    UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InStackTag, // 등록할 스택 태그
    UCommonActivatableWidgetContainerBase* InStack                               // 등록할 위젯 스택
)
{
    // 디자인 타임(에디터 미리보기) 상태가 아니면 실행
    if (!IsDesignTime())
    {
        // 같은 태그가 이미 등록되어 있지 않으면 새로 추가
        if (!RegisteredWidgetStackMap.Contains(InStackTag))
        {
            RegisteredWidgetStackMap.Add(InStackTag, InStack);
        }
    }
}

