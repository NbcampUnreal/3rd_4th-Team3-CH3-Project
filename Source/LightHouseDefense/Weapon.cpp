// Fill out your copyright notice in the Description page of Project Settings.



#include "Weapon.h"
#include "WeaponProjectile.h"
#include "LighthouseHUD.h"                 // [ADD]
#include "GameFramework/PlayerController.h"// [ADD] GetHUD()용
#include "UObject/UnrealType.h"
#include "CHCharacter.h"
#include "E_WeaponType.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UWeaponUnlockSubsystem.h"
#include "DrawDebugHelpers.h"              // 디버그 라인/포인트
#include "Components/TextBlock.h"          // [ADD] UTextBlock 사용

static bool IsBPEnumEqualByName(const UObject* Obj, FName VarName, const TCHAR* WantedName)
{
    if (!Obj) return false;

    if (const FEnumProperty* EP = FindFProperty<FEnumProperty>(Obj->GetClass(), VarName))
    {
        const void* Ptr = EP->ContainerPtrToValuePtr<void>(Obj);
        const int64 Raw = EP->GetUnderlyingProperty()->GetSignedIntPropertyValue(Ptr);
        const UEnum* En = EP->GetEnum();
        if (!En) return false;

        const FString ByName = En->GetNameStringByValue(Raw);
        const FString ByDisp = En->GetDisplayNameTextByValue(Raw).ToString();
        return ByName.Equals(WantedName, ESearchCase::IgnoreCase) ||
            ByDisp.Equals(WantedName, ESearchCase::IgnoreCase);
    }

    if (const FByteProperty* BP = FindFProperty<FByteProperty>(Obj->GetClass(), VarName))
    {
        const void* Ptr = BP->ContainerPtrToValuePtr<void>(Obj);
        const uint8 Raw = BP->GetPropertyValue(Ptr);
        const UEnum* En = BP->Enum;
        if (!En) return false;

        const FString ByName = En->GetNameStringByValue(Raw);
        const FString ByDisp = En->GetDisplayNameTextByValue(Raw).ToString();
        return ByName.Equals(WantedName, ESearchCase::IgnoreCase) ||
            ByDisp.Equals(WantedName, ESearchCase::IgnoreCase);
    }

    return false;
}

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    if (!GetRootComponent())
    {
        USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
        SetRootComponent(Root);
    }

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

    OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
    OnReloadChanged.Broadcast(bIsReloading);
    OnCanFireChanged.Broadcast(bCanFire);
    UpdateAmmoUI();
}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeapon::SetAmmoTextBlock(UTextBlock* InText)
{
    AmmoTextBlock = InText;
    UpdateAmmoUI();
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
            AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
            EquippedSocketName = SocketName;
        }
    }

    SetActorEnableCollision(true);
    SetActorHiddenInGame(false);

    if (AController* C = NewOwnerPawn->GetController())
    {
        if (APlayerController* PC = Cast<APlayerController>(C))
        {
            if (ALighthouseHUD* HUD = PC->GetHUD<ALighthouseHUD>())
            {
                HUD->RebindAmmoToCurrentWeapon();
            }
        }
    }

    if (APlayerController* PC = Cast<APlayerController>(NewOwnerPawn->GetController()))
        if (ALighthouseHUD* HUD = PC->GetHUD<ALighthouseHUD>())
            HUD->RebindAmmoToCurrentWeapon();

    UpdateAmmoUI();
}

void AWeapon::Reload()
{
    if (bIsReloading)                return;
    if (ReserveAmmo <= 0)            return;
    if (CurrentAmmo >= MagazineSize) return;

    bIsReloading = true;
    OnReloadStarted();
    OnReloadChanged.Broadcast(true);

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
    OnReloadChanged.Broadcast(false);
    OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
    UpdateAmmoUI();
}

void AWeapon::StartFireCooldown()
{
    bCanFire = false;
    OnCanFireChanged.Broadcast(false);

    const float Interval = (FireRate > 0.f) ? (1.f / FireRate) : 0.1f;
    GetWorldTimerManager().SetTimer(
        TH_FireCooldown,
        [this]()
        {
            bCanFire = true;
            OnCanFireChanged.Broadcast(true);
        },
        Interval,
        false
    );
}

void AWeapon::Fire()
{
    // === 무기 해금 체크 ===
    {
        UWeaponUnlockSubsystem* Unlock = GetGameInstance()
            ? GetGameInstance()->GetSubsystem<UWeaponUnlockSubsystem>()
            : nullptr;

        if (!Unlock)
        {
            if (WeaponType != E_WeaponType::Pistol)
            {
                UE_LOG(LogTemp, Warning, TEXT("[Weapon] UnlockSubsystem missing. Block fire for %d"), (int32)WeaponType);
                return;
            }
        }
        else
        {
            if (!Unlock->IsUnlocked(WeaponType))
            {
                UE_LOG(LogTemp, Warning, TEXT("[Weapon] Locked weapon. Need kills. Type=%d"), (int32)WeaponType);
                return;
            }
        }
    }

    const ACHCharacter* CH = Cast<ACHCharacter>(GetOwner());
    if (!CH) return;

    if (CH->GetCurrentWeaponType() == E_WeaponType::Hand)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Fire blocked: Hand state"));
        return;
    }

    if (CH->GetCurrentWeapon() != this) return;

    if (!bCanFire || bIsReloading) return;

    if (CurrentAmmo <= 0)
    {
        if (bAutoReload && ReserveAmmo > 0)
        {
            Reload();
        }
        else
        {
            OnDryFire();
        }
        return;
    }

    // 카메라 위치/방향
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
        CamLoc = Muzzle ? Muzzle->GetComponentLocation() : GetActorLocation();
        CamRot = GetActorRotation();
    }

    const float TraceDist = FMath::Max(Range, 1000.f);
    const ECollisionChannel TraceChannel = ECC_Visibility;

    // 히트스캔 (노란 점만 표시, 선은 없음) 
    if (PelletCount > 1) // 샷건
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
                    GetInstigatorController(), this, nullptr
                );

                // 선은 제거, 맞은 곳에 점만
                DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 6.f, FColor::Yellow, false, 1.2f);
            }
            
        }
    }
    else // 단일
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
                GetInstigatorController(), this, nullptr
            );

            // 선은 제거, 맞은 곳에 점만
            DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.f, FColor::Yellow, false, 1.2f);
        }
        
    }

    // 후처리
    PlayFireEffect();
    CurrentAmmo = FMath::Max(0, CurrentAmmo - 1);
    OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
    UpdateAmmoUI();
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

void AWeapon::UpdateAmmoUI()
{
    if (AmmoTextBlock.IsValid())
    {
        const FText AmmoTxt = FText::Format(
            NSLOCTEXT("Weapon", "AmmoFmt", "{0} / {1}"),
            FText::AsNumber(CurrentAmmo),
            FText::AsNumber(ReserveAmmo)
        );
        AmmoTextBlock->SetText(AmmoTxt);
    }
}

