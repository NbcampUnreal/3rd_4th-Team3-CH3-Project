// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "WeaponProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h" //발사체 속도 접근
#include "Components/SceneComponent.h"  //Muzzle
#include "GameFramework/Pawn.h" //Instigator
#include "Components/SkeletalMeshComponent.h" //소켓 부착
#include "Engine/World.h"


AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

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
    SetActorHiddenInGame(true);
}

void AWeapon::Fire()
{
    FVector CamLoc;
    FRotator CamRot;

    // 1인칭 카메라 기준 
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    // 플레이어 카메라 기준 발사
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    const FVector SpawnLoc = (Muzzle ? Muzzle->GetComponentLocation() : CamLoc); // ✔ 세미콜론
    const FRotator SpawnRot = CamRot;

    if (ProjectileClass)
    {
        // 스폰 파라미터 설정 Owner,Instigator 충돌 처리
        FActorSpawnParameters Params;
        Params.Owner = this;                // 피해소유자
        Params.Instigator = GetInstigator();// 가해자(컨트롤된 Pawn)
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // 스폰
        AWeaponProjectile* Projectile =
            GetWorld()->SpawnActor<AWeaponProjectile>(ProjectileClass, CamLoc, CamRot, Params);

        if (Projectile)
        {
            const FVector FireDir = SpawnRot.Vector();

           
            if (Projectile->ProjectileMovement)
            {
                Projectile->ProjectileMovement->Velocity = FireDir * 3000.f;
            }

            //무기 데미지를 탄환으로 전달 projectille에 데미지 가 있으면
            Projectile->Damage = Damage;
        }
    }

    // 총구 화염/ 사운드 (bp에서 구현)
    PlayFireEffect();
}



