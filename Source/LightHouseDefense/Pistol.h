// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Pistol.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API APistol : public AWeapon
{
    GENERATED_BODY()

public:
    APistol();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    virtual void Fire() override;
};

