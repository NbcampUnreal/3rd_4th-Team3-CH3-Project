#pragma once

#include "CoreMinimal.h"
#include "E_WeaponType.generated.h"

UENUM(BlueprintType)
enum class E_WeaponType : uint8
{
    None UMETA(DisplayName = "None"),
    AssasultRifle UMETA(DisplayName = "AssasultRifle"),
    AssasultRifle2 UMETA(DisplayName = "AssasultRifle2"),
    Shotgun UMETA(DisplayName = "ShotGun"),
    Pistol UMETA(DisplayName = "Pistol"),
    SniperRifle UMETA(DisplayName = "SniperRifle")
};
