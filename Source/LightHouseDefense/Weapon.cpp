// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "WeaponProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h" // 디버그 라인/포인트

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    // 루트 없으면 생성
    if (!GetRootComponent())
    {
        USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
        SetRootComponent(Root);
    }

    // 총구(Muzzle)
    Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
    if (Muzzle)
    {
        Muzzle->SetupAttachment(GetRootComponent());
    }

    SetActorEnableCollision(false);
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    CurrentAmmo = FMath::Clamp(CurrentAmmo, 0, MagazineSize);
}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeapon::Equip(APawn* NewOwnerPawn, FName SocketName)
{
    if (!NewOwnerPawn) return;

    SetOwner(NewOwnerPawn);
    SetInstigator(NewOwnerPawn);

    if (USkeletalMeshComponent* Mesh = NewOwnerPawn->FindComponentByClass<USkeletalMeshComponent>())
    {
        if (Mesh->DoesSocketExist(SocketName))
        {
            AttachToComponent(Mesh,
                FAttachmentTransformRules::SnapToTargetIncludingScale,
                SocketName);
            EquippedSocketName = SocketName;
        }
    }

    SetActorEnableCollision(true);
    SetActorHiddenInGame(false);
}

void AWeapon::Reload()
{
    if (bIsReloading)                return;
    if (ReserveAmmo <= 0)            return;
    if (CurrentAmmo >= MagazineSize) return;

    bIsReloading = true;
    OnReloadStarted();

    GetWorldTimerManager().SetTimer(TH_Reload, this, &AWeapon::FinishReload, ReloadTime, false);
}

void AWeapon::FinishReload()
{
    bIsReloading = false;

    const int32 Need = MagazineSize - CurrentAmmo;
    const int32 ToLoad = FMath::Min(Need, ReserveAmmo);

    CurrentAmmo += ToLoad;
    ReserveAmmo -= ToLoad;

    OnReloadFinished();
}

void AWeapon::StartFireCooldown()
{
    bCanFire = false;
    const float Interval = (FireRate > 0.f) ? (1.f / FireRate) : 0.1f;
    GetWorldTimerManager().SetTimer(TH_FireCooldown, [this]()
        {
            bCanFire = true;
        }, Interval, false);
}

void AWeapon::Fire()
{
    // 가능 여부
    if (!bCanFire || bIsReloading) return;

    // 탄 없음 → 자동재장전 옵션/드라이파이어
    if (CurrentAmmo <= 0)
    {
        if (bAutoReload && ReserveAmmo > 0)   // [ADD]
        {
            Reload();                         // [ADD] 자동 재장전이 켜진 경우에만
        }
        else
        {
            OnDryFire();                      // [ADD] 딸깍(사운드/UI) - BP에서 처리
        }
        return;
    }

    // 카메라 위치/방향 (Instigator 우선)
    FVector  CamLoc = FVector::ZeroVector;
    FRotator CamRot = FRotator::ZeroRotator;

    if (AController* C = GetInstigatorController())
    {
        C->GetPlayerViewPoint(CamLoc, CamRot);
    }
    else if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->GetPlayerViewPoint(CamLoc, CamRot);
    }
    else
    {
        // 폴백: 무기 기준
        CamLoc = Muzzle ? Muzzle->GetComponentLocation() : GetActorLocation();
        CamRot = GetActorRotation();
    }

    // 사거리(최소 가드)
    const float TraceDist = FMath::Max(Range, 1000.f); // [MOD]
    const ECollisionChannel TraceChannel = ECC_Visibility;

    // ===== 샷건(다중) / 일반(단일) 분기 =====
    if (PelletCount > 1) // 샷건 모드
    {
        for (int32 i = 0; i < PelletCount; ++i)
        {
            FRotator ShotRot = CamRot;
            if (SpreadAngle > 0.f)
            {
                ShotRot.Pitch += FMath::FRandRange(-SpreadAngle, SpreadAngle);
                ShotRot.Yaw += FMath::FRandRange(-SpreadAngle, SpreadAngle);
            }

            const FVector TraceEnd = CamLoc + ShotRot.Vector() * TraceDist;

            FHitResult Hit;
            FCollisionQueryParams QP(SCENE_QUERY_STAT(ShotgunTrace), true);
            QP.AddIgnoredActor(this);
            if (GetOwner()) QP.AddIgnoredActor(GetOwner());

            if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, TraceChannel, QP))
            {
                UGameplayStatics::ApplyPointDamage(
                    Hit.GetActor(), Damage, ShotRot.Vector(), Hit,
                    GetInstigatorController(), this, nullptr);

                DrawDebugLine(GetWorld(), CamLoc, Hit.ImpactPoint, FColor::Red, false, 1.2f, 0, 1.2f);
                DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 6.f, FColor::Yellow, false, 1.2f);
            }
            else
            {
                DrawDebugLine(GetWorld(), CamLoc, TraceEnd, FColor::Blue, false, 1.2f, 0, 1.2f);
            }
        }
    }
    else // 일반 무기: 단일 레이
    {
        const FVector TraceEnd = CamLoc + CamRot.Vector() * TraceDist;

        FHitResult Hit;
        FCollisionQueryParams QP(SCENE_QUERY_STAT(WeaponFireTrace), true);
        QP.AddIgnoredActor(this);
        if (GetOwner()) QP.AddIgnoredActor(GetOwner());

        if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, TraceChannel, QP))
        {
            UGameplayStatics::ApplyPointDamage(
                Hit.GetActor(), Damage, CamRot.Vector(), Hit,
                GetInstigatorController(), this, nullptr);

            DrawDebugLine(GetWorld(), CamLoc, Hit.ImpactPoint, FColor::Red, false, 1.2f, 0, 1.2f);
            DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.f, FColor::Yellow, false, 1.2f);
        }
        else
        {
            DrawDebugLine(GetWorld(), CamLoc, TraceEnd, FColor::Blue, false, 1.2f, 0, 1.2f);
        }
    }

    // 후처리
    PlayFireEffect();
    CurrentAmmo = FMath::Max(0, CurrentAmmo - 1);
    StartFireCooldown();
}

void AWeapon::Unequip()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetOwner(nullptr);
    SetInstigator(nullptr);
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}




