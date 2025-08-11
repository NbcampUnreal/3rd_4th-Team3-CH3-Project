// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "CommonTextBlock.h"

void UFrontendCommonButtonBase::SetButtonText(FText InText)
{
    // 버튼 텍스트 블록이 존재하고, 전달된 텍스트가 비어있지 않으면
    if (CommonTextBlock_ButtonText && !InText.IsEmpty())
    {
        // 설정에 따라 텍스트를 대문자로 변환하거나 그대로 설정
        CommonTextBlock_ButtonText->SetText(
            bUserUpperCaseForButtonText ? InText.ToUpper() : InText
        );
    }
}

void UFrontendCommonButtonBase::NativePreConstruct()
{
    // 부모 클래스의 NativePreConstruct 실행 (위젯 초기화 로직 유지)
    Super::NativePreConstruct();

    // 버튼 표시 텍스트(ButtonDisplayText)로 초기 버튼 텍스트 설정
    SetButtonText(ButtonDisplayText);
}

