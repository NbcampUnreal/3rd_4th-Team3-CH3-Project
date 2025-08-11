// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace FrontendGameplayTags // Frontend 관련 Gameplay Tag들을 모아둔 네임스페이스
{
    //Frontend 위젯 스택 태그 선언
    LIGHTHOUSEDEFENSE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_Modal);     // 모달 팝업 UI 스택 태그
    LIGHTHOUSEDEFENSE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_GameMenu);  // 인게임 메뉴 UI 스택 태그
    LIGHTHOUSEDEFENSE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_GameHud);   // HUD UI 스택 태그
    LIGHTHOUSEDEFENSE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_Frontend);  // 프론트엔드 UI 스택 태그

    // Frontend 개별 위젯 태그 선언
    LIGHTHOUSEDEFENSE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Widget_PressAnyKeyScreen); // "아무 키나 누르세요" 화면 태그
    LIGHTHOUSEDEFENSE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Widget_MainMenuScreen);    // 메인 메뉴 화면 태그
}
