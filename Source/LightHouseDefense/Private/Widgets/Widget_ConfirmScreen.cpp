// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Widget_ConfirmScreen.h"
#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "ICommonInputModule.h"

/**
 * OK(단일) 확인창용 데이터 오브젝트 생성
 * - 제목/본문 텍스트를 세팅하고, "Ok" 버튼 1개만 추가
 * - 버튼 타입은 Closed (닫기)로 지정
 */
UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOKScreen(const FText& InScreenTitle, const FText& InScreenMsg)
{
    UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();
    InfoObject->ScreenTitle = InScreenTitle;   // 화면 상단 제목
    InfoObject->ScreenMessage = InScreenMsg;   // 본문 메시지

    FConfirmScreenButtonInfo OKButtonInfo;
    OKButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Closed; // OK 클릭 시 '닫힘' 의미
    OKButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Ok"));        // 버튼 라벨

    InfoObject->AvailableScreenButtons.Add(OKButtonInfo); // 버튼 목록에 등록

    return InfoObject;
}

/**
 * 예/아니오(Yes/No) 확인창용 데이터 오브젝트 생성
 * - "Yes"(Confirmed), "No"(Cancelled) 두 버튼 추가
 */
UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateYesNoScreen(const FText& InScreenTitle, const FText& InScreenMsg)
{
    UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();
    InfoObject->ScreenTitle = InScreenTitle;
    InfoObject->ScreenMessage = InScreenMsg;

    FConfirmScreenButtonInfo YesButtonInfo;
    YesButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed; // 수락/확정
    YesButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Yes"));

    FConfirmScreenButtonInfo NoButtonInfo;
    NoButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;  // 취소
    NoButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("No"));

    InfoObject->AvailableScreenButtons.Add(YesButtonInfo);
    InfoObject->AvailableScreenButtons.Add(NoButtonInfo);

    return InfoObject;
}

/**
 * 확인/취소(Ok/Cancel) 확인창용 데이터 오브젝트 생성
 * - "Ok"(Confirmed), "Cancel"(Cancelled) 두 버튼 추가
 */
UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOkCancelScreen(const FText& InScreenTitle, const FText& InScreenMsg)
{
    UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();
    InfoObject->ScreenTitle = InScreenTitle;
    InfoObject->ScreenMessage = InScreenMsg;

    FConfirmScreenButtonInfo OkButtonInfo;
    OkButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed; // 확인
    OkButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Ok"));

    FConfirmScreenButtonInfo CancelButtonInfo;
    CancelButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled; // 취소
    CancelButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Cancel"));

    InfoObject->AvailableScreenButtons.Add(OkButtonInfo);
    InfoObject->AvailableScreenButtons.Add(CancelButtonInfo);

    return InfoObject;
}

/**
 * 확인창 위젯 초기화
 * - 전달받은 InfoObject(제목/본문/버튼 구성)에 맞게 화면 세팅
 * - 기존 버튼 엔트리의 OnClicked 바인딩을 모두 클리어하고, 새로 버튼을 생성/배치
 * - CommonInput의 기본 액션(Confirm/Back)을 버튼에 매핑하여 패드/키보드 아이콘 표시 연동
 * - 클릭 시 콜백 호출 후 현재 위젯 비활성화
 * - 마지막 버튼에 포커스 설정(키보드/패드 내비게이션 편의)
 *
 * @param InScreenInfoObject    화면에 표시할 텍스트/버튼 구성 데이터
 * @param ClickedButtonCallback 버튼 클릭 시 실행할 콜백(버튼 타입 전달)
 */
void UWidget_ConfirmScreen::InitConfirmScreen(UConfirmScreenInfoObject* InScreenInfoObject, TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallback)
{
    // 필수 위젯 및 데이터 체크(개발 단계에서 문제 조기 발견)
    check(InScreenInfoObject && CommonTextBlock_Title && CommonTextBlock_Message && DynamicEntryBox_Buttons);

    // 제목/본문 표시
    CommonTextBlock_Title->SetText(InScreenInfoObject->ScreenTitle);
    CommonTextBlock_Message->SetText(InScreenInfoObject->ScreenMessage);

    // 기존에 생성된 버튼 엔트리가 있으면 모두 초기화(중복 바인딩/메모리 누수 방지)
    if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
    {
        // Reset<T> 호출 시 각 엔트리에 대해 제공한 람다 실행
        // → 기존 버튼의 OnClicked 델리게이트를 모두 Clear하여 이벤트 중복 호출 방지
        DynamicEntryBox_Buttons->Reset<UFrontendCommonButtonBase>(
            [](UFrontendCommonButtonBase& ExistingButton)
            {
                ExistingButton.OnClicked().Clear(); // 이전 바인딩 제거
            }
        );
    }

    // 버튼 데이터가 최소 1개 이상 존재해야 함
    check(!InScreenInfoObject->AvailableScreenButtons.IsEmpty());

    // 버튼 데이터 목록을 순회하며 실제 버튼 위젯 생성/설정
    for (const FConfirmScreenButtonInfo& AvailableButtonInfo : InScreenInfoObject->AvailableScreenButtons)
    {
        FDataTableRowHandle InputActionRowHandle; // CommonInput 액션(아이콘/키가이드) 매핑용

        // 버튼 타입에 따라 기본 입력 액션(확인/뒤로) 매핑
        switch (AvailableButtonInfo.ConfirmScreenButtonType)
        {
        case EConfirmScreenButtonType::Confirmed:
            // 보통 A/Enter 등 '확인' 액션(아이콘/키가이드 표시)
            InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultClickAction();
            break;

        case EConfirmScreenButtonType::Cancelled:
            // 보통 B/Esc 등 '뒤로' 액션
            InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();
            break;

        case EConfirmScreenButtonType::Closed:
            // 단일 OK 같은 '닫기'도 뒤로 액션으로 처리(키가이드 일관성)
            InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();
            break;

        default:
            break;
        }

        // 버튼 엔트리 동적 생성(EntryBox 슬롯에 추가)
        UFrontendCommonButtonBase* AddedButton = DynamicEntryBox_Buttons->CreateEntry<UFrontendCommonButtonBase>();
        AddedButton->SetButtonText(AvailableButtonInfo.ButtonTextToDisplay); // 라벨 텍스트 적용
        AddedButton->SetTriggeredInputAction(InputActionRowHandle);          // 입력 액션(키가이드/아이콘) 연결

        // 클릭 이벤트 바인딩: 외부로 콜백 전달 후 위젯 비활성화
        // 캡처: 콜백, 버튼정보(값 복사), this(위젯 인스턴스)
        AddedButton->OnClicked().AddLambda(
            [ClickedButtonCallback, AvailableButtonInfo, this]()
            {
                // 어떤 버튼이 눌렸는지 타입을 넘겨줌(Confirmed/Cancelled/Closed)
                ClickedButtonCallback(AvailableButtonInfo.ConfirmScreenButtonType);

                // 확인창 닫기(현재 위젯 비활성화)
                DeactivateWidget();
            }
        );
    }

    // 마지막으로 추가된 버튼에 포커스 설정(키보드/패드 사용자 편의)
    if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
    {
        DynamicEntryBox_Buttons->GetAllEntries().Last()->SetFocus();
    }
}
