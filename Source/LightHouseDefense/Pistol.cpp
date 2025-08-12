// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

APistol::APistol()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APistol::BeginPlay()
{
    Super::BeginPlay();
}

void APistol::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APistol::Fire()
{
    FVector Start;
    FRotator Rotation;

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController) return;

    PlayerController->GetPlayerViewPoint(Start, Rotation);

    const FVector End = Start + Rotation.Vector() * Range;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    if (bHit)
    {
        UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, Rotation.Vector(), Hit, nullptr, this, nullptr);
        DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.f, FColor::Red, false, 1.0f);
    }

    // 부모에 선언된 BP 이벤트 호출(총구화염/사운드 등)
    PlayFireEffect();
}


