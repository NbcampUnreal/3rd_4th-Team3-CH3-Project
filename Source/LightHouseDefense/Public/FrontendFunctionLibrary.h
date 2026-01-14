// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "FrontendFunctionLibrary.generated.h"

class UWidget_ActivatableBase;
/**
 *
 */
UCLASS() // 언리얼 리플렉션 시스템에 등록되는 클래스임을 표시
class LIGHTHOUSEDEFENSE_API UFrontendFunctionLibrary : public UBlueprintFunctionLibrary // 블루프린트에서 호출 가능한 함수 모음 클래스
{
    GENERATED_BODY() // 언리얼 매크로: 리플렉션 및 기본 생성 코드 자동 생성

public:
    UFUNCTION(BlueprintPure, Category = "Frontend Function Library") // 블루프린트에서 읽기 전용 함수로 호출 가능, 카테고리는 "Frontend Function Library"
        static TSoftClassPtr<UWidget_ActivatableBase> GetFrontendSoftWidgetClassByTag( // 태그를 기반으로 SoftClass 위젯을 반환하는 정적 함수
            UPARAM(meta = (Categories = "Frontend.Widget")) FGameplayTag InWidgetTag // Frontend.Widget 카테고리의 GameplayTag 입력 파라미터
        );
};


