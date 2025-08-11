#pragma once

#include "CoreMinimal.h"
#include "CHWeaponEnums.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Rifle     UMETA(DisplayName = "Rifle"),
    Shotgun   UMETA(DisplayName = "Shotgun"),
    None    UMETA(DisplayName = "None")
};
