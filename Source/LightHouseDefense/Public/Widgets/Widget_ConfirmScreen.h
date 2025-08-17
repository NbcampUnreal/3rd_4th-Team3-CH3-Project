// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "FrontendType/FrontendEnumTypes.h"
#include "Widget_ConfirmScreen.generated.h"

class UCommonTextBlock;
class UDynamicEntryBox;

USTRUCT(BlueprintType)
struct FConfirmScreenButtonInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EConfirmScreenButtonType ConfirmScreenButtonType = EConfirmScreenButtonType::Unknown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ButtonTextToDisplay;
};

UCLASS()
class LIGHTHOUSEDEFENSE_API UConfirmScreenInfoObject : public UObject
{
    GENERATED_BODY()

public:
    static UConfirmScreenInfoObject* CreateOKScreen(const FText& InScreenTitle, const FText& InScreenMsg);
    static UConfirmScreenInfoObject* CreateYesNoScreen(const FText& InScreenTitle, const FText& InScreenMsg);
    static UConfirmScreenInfoObject* CreateOkCancelScreen(const FText& InScreenTitle, const FText& InScreenMsg);

    UPROPERTY(Transient)
    FText ScreenTitle;

    UPROPERTY(Transient)
    FText ScreenMessage;

    UPROPERTY(Transient)
    TArray<FConfirmScreenButtonInfo> AvailableScreenButtons;
};

/**
 *
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class LIGHTHOUSEDEFENSE_API UWidget_ConfirmScreen : public UWidget_ActivatableBase
{
    GENERATED_BODY()

public:
    // 위젯이 생성되고, 모달 스택에 푸시되기 전에 클래스 외부에서 호출
    void InitConfirmScreen(UConfirmScreenInfoObject* InScreenInfoObject, TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallback);

private:
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* CommonTextBlock_Title;

    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* CommonTextBlock_Message;

    UPROPERTY(meta = (BindWidget))
    UDynamicEntryBox* DynamicEntryBox_Buttons;
};
