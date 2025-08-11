// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FrontendCommonButtonBase.generated.h"

class UCommonTextBlock;

/**
 *
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick)) // 추상 클래스, 블루프린트에서 생성 가능, 기본 Tick 비활성화
class LIGHTHOUSEDEFENSE_API UFrontendCommonButtonBase : public UCommonButtonBase // Frontend 전용 버튼 기본 클래스, CommonButtonBase 확장
{
    GENERATED_BODY() // 언리얼 리플렉션 및 기본 생성 코드 자동 생성

public:
    UFUNCTION(BlueprintCallable) // 블루프린트에서 호출 가능
        void SetButtonText(FText InText); // 버튼 표시 텍스트 설정 (옵션에 따라 대문자 변환 가능)

private:
    //~ Begin UUserWidget Interface
    virtual void NativePreConstruct() override; // 위젯이 생성되기 전에 호출되는 초기화 함수
    //~ End UUserWidget Interface

    // **** Bound Widgets **** //
    UPROPERTY(meta = (BindWidgetOptional)) // 블루프린트 위젯에서 연결 가능, 없더라도 동작
        UCommonTextBlock* CommonTextBlock_ButtonText; // 버튼 표시 텍스트 블록 위젯
    // **** Bound Widgets **** //

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Button", meta = (AllowPrivateAccess = "true"))
    FText ButtonDisplayText; // 버튼 기본 표시 텍스트

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Button", meta = (AllowPrivateAccess = "true"))
    bool bUserUpperCaseForButtonText = false; // 버튼 텍스트를 대문자로 변환할지 여부

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Button", meta = (AllowPrivateAccess = "true"))
    FText ButtonDescriptionText; // 버튼 설명 텍스트
};

