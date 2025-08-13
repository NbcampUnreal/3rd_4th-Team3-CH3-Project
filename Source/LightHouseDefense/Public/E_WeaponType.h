#pragma once

#include "CoreMinimal.h"
#include "E_WeaponType.generated.h"

UENUM(BlueprintType)
enum class E_WeaponType : uint8
{
    None UMETA(DisplayName = "None"),
    AK47 UMETA(DisplayName = "AK47"),
    M16 UMETA(DisplayName = "M16"),
    Shotgun UMETA(DisplayName = "ShotGun"),
    Pistol UMETA(DisplayName = "Pistol"),
    SniperRifle UMETA(DisplayName = "SniperRifle")
};
