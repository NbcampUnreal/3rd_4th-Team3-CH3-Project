// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Widget_ActivatableBase.generated.h"

/**
 *
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick)) // 추상 클래스, 블루프린트에서 생성 가능, 기본 Tick 비활성화
class LIGHTHOUSEDEFENSE_API UWidget_ActivatableBase : public UCommonActivatableWidget // 공통 활성화 가능한 위젯의 기본 클래스
{
    GENERATED_BODY()
};

