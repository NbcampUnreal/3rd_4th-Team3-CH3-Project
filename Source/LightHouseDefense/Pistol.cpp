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

    // 컨트롤러로부터 위치와 방향을 얻음
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
            // 데미지 적용
            UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, Rotation.Vector(), Hit, nullptr, this, nullptr);

            // 디버그 히트 위치 표시
            DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10, FColor::Red, false, 1.0f);
        }

        // 발사 이펙트 
        PlayFireEffect();
    }
}
