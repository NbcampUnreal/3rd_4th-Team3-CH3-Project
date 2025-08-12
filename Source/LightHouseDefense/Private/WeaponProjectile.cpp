// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AWeaponProjectile::AWeaponProjectile()
{
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(5.0f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComp->SetNotifyRigidBodyCollision(true);
    RootComponent = CollisionComp;

    CollisionComp->OnComponentHit.AddDynamic(this, &AWeaponProjectile::OnHit);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;

    InitialLifeSpan = 3.0f;
}

void AWeaponProjectile::OnHit(UPrimitiveComponent* HitComp,
                              AActor* OtherActor,
                              UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse,
                              const FHitResult& Hit)
{
    // 충돌 후 처리
    if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
    {
        Destroy();
    }
}

