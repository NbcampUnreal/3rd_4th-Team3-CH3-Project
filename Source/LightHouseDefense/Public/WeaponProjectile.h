// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UPrimitiveComponent;

UCLASS()
class LIGHTHOUSEDEFENSE_API AWeaponProjectile : public AActor
{
    GENERATED_BODY()

public:
    AWeaponProjectile();

    UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
    class USphereComponent* CollisionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    class UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float Damage = 0.0f;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp,
               AActor* OtherActor,
               UPrimitiveComponent* OtherComp,
               FVector NormalImpulse,
               const FHitResult& Hit);
};

