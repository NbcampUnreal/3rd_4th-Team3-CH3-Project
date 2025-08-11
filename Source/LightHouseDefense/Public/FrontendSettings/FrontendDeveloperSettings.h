
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "FrontendDeveloperSettings.generated.h"

class UWidget_ActivatableBase;
/**
 *
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Frontend UI Settings")) // 게임 설정 파일(Game.ini)에 저장되는 Frontend UI 설정 클래스, 에디터에서 "Frontend UI Settings" 이름으로 표시
class LIGHTHOUSEDEFENSE_API UFrontendDeveloperSettings : public UDeveloperSettings // 개발자가 에디터에서 쉽게 접근/수정할 수 있는 설정 클래스
{
    GENERATED_BODY() // 언리얼 리플렉션 및 기본 생성 코드 자동 생성

public:
    UPROPERTY( // 속성 메타데이터 정의
        Config, // Game.ini 등 설정 파일에 저장
        EditAnywhere, // 에디터 어디서든 수정 가능
        Category = "Widget Reference", // 에디터 속성 카테고리 이름
        meta = (ForceInlineRow, Categories = "Frontend.Widget") // 세부 설정: 맵 항목을 한 줄에 표시, Frontend.Widget 카테고리 태그만 허용
    )
    TMap<FGameplayTag, TSoftClassPtr<UWidget_ActivatableBase>> FrontendWidgetMap; // Frontend 위젯 태그와 해당 SoftClass UI 위젯을 매핑하는 설정 값
};

