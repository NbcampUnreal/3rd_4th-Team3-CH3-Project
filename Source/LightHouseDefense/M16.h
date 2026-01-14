// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "M16.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API AM16 : public AWeapon
{
    GENERATED_BODY()

public:
    AM16();

protected:
    virtual void BeginPlay() override;
};
