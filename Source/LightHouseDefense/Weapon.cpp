// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeapon::Fire()
{
    FVector Start;
    FRotator Rotation;

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController)
    {
        PlayerController->GetPlayerViewPoint(Start, Rotation);

        FVector End = Start + Rotation.Vector() * Range;

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

        if (bHit)
        {
            UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, Rotation.Vector(), Hit, nullptr, this, nullptr);
            DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10, FColor::Red, false, 1.0f);
        }

        PlayFireEffect();
    }
}


