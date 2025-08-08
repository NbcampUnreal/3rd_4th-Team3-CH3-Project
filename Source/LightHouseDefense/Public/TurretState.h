#pragma once

#include "CoreMinimal.h"

// 터렛의 상태를 정의하는 Enum
UENUM(BlueprintType)
enum class ETurretState : uint8
{
    Disabled UMETA(DisplayName = "Disabled"),
    Scanning UMETA(DisplayName = "Scanning"),
    Attacking UMETA(DisplayName = "Attacking"),
    // 필요한 경우 다른 상태를 추가할 수 있습니다.
};
