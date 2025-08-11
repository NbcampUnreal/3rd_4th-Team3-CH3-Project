// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontendFunctionLibrary.h"
#include "FrontendSettings/FrontendDeveloperSettings.h"

TSoftClassPtr<UWidget_ActivatableBase> UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(
    UPARAM(meta = (Categories = "Frontend.Widget")) FGameplayTag InWidgetTag // 찾고자 하는 위젯의 태그
)
{
    // Frontend 개발 설정 객체 가져오기 (기본 설정값 참조)
    const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();

    // 전달받은 태그가 FrontendWidgetMap에 존재하는지 확인
    checkf(
        FrontendDeveloperSettings->FrontendWidgetMap.Contains(InWidgetTag),
        TEXT("Could not find the corresponding widget under the tag %s"), *InWidgetTag.ToString()
    );

    // 태그에 해당하는 소프트 위젯 클래스 반환
    return FrontendDeveloperSettings->FrontendWidgetMap.FindRef(InWidgetTag);
}

