// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "WeaponProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h" //발사체 속도 접근
#include "Components/SceneComponent.h"  //Muzzle
#include "GameFramework/Pawn.h" //Instigator
#include "Components/SkeletalMeshComponent.h" //소켓 부착
#include "Engine/World.h"
#include "TimerManager.h"


AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    if (!GetRootComponent())
    {
        USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
        SetRootComponent(Root);
    }

    //머즐 컴포넌트 생성
    Muzzle = CreateDefaultSubobject< USceneComponent>(TEXT("Muzzle"));
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
            AttachToComponent(
                Mesh,
                FAttachmentTransformRules::SnapToTargetIncludingScale,                          
                SocketName
            );

            EquippedSocketName = SocketName;

        }
    }

    SetActorEnableCollision(true);
    SetActorHiddenInGame(false);
}


void AWeapon::Reload() // ★ 수동 재장전
{
    if (bIsReloading)              return;
    if (ReserveAmmo <= 0)          return;
    if (CurrentAmmo >= MagazineSize) return;

    bIsReloading = true;
    OnReloadStarted(); //  BP 애니/사운드 훅

    GetWorldTimerManager().SetTimer(TH_Reload, this, &AWeapon::FinishReload, ReloadTime, false);
}

void AWeapon::FinishReload() //  재장전 완료 콜백
{
    bIsReloading = false;

    const int32 Need = MagazineSize - CurrentAmmo;
    const int32 ToLoad = FMath::Min(Need, ReserveAmmo);

    CurrentAmmo += ToLoad;
    ReserveAmmo -= ToLoad;

    OnReloadFinished(); //  BP 애니/사운드 훅
}

void AWeapon::StartFireCooldown() //  발사 쿨다운
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
    if (!bCanFire || bIsReloading) return;
    if (CurrentAmmo <= 0)
    {
        if (ReserveAmmo > 0) Reload();
        return;
    }

    FVector CamLoc;
    FRotator CamRot;
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->GetPlayerViewPoint(CamLoc, CamRot);
    }

    const float TraceDist = 20000.f;
    const FVector TraceEnd = CamLoc + CamRot.Vector() * TraceDist;

    // 라인트레이스 파라미터
    FHitResult Hit;
    FCollisionQueryParams QP(SCENE_QUERY_STAT(WeaponFireTrace), true);
    QP.AddIgnoredActor(this);
    if (GetOwner()) QP.AddIgnoredActor(GetOwner());

    // 라인트레이스 실행
    if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, QP))
    {
        // 맞은 액터에 데미지 적용
        UGameplayStatics::ApplyPointDamage(
            Hit.GetActor(),
            Damage,
            CamRot.Vector(),
            Hit,
            GetInstigatorController(),
            this,
            nullptr
        );

        // 디버그용 라인
        DrawDebugLine(GetWorld(), CamLoc, Hit.ImpactPoint, FColor::Red, false, 1.0f, 0, 1.0f);
    }
    else
    {
        DrawDebugLine(GetWorld(), CamLoc, TraceEnd, FColor::Blue, false, 1.0f, 0, 1.0f);
    }

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



