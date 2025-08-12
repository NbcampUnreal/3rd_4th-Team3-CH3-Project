// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "WeaponProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

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

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    // 플레이어 카메라 기준 발사
    PC->GetPlayerViewPoint(Start, Rotation);

    if (ProjectileClass)
    {
        // ★ 추가: 스폰 파라미터 설정 (데미지 소유자/가해자 추적에 중요)
        FActorSpawnParameters Params;
        Params.Owner = this;                // 피해소유자
        Params.Instigator = GetInstigator();// 가해자(컨트롤된 Pawn)

        // 스폰
        AWeaponProjectile* Projectile =
            GetWorld()->SpawnActor<AWeaponProjectile>(ProjectileClass, Start, Rotation, Params);

        if (Projectile)
        {
            const FVector FireDir = Rotation.Vector();

            // ★ 변경: GetProjectileMovement() 대신 멤버 직접 접근
            if (Projectile->ProjectileMovement)
            {
                Projectile->ProjectileMovement->Velocity = FireDir * 3000.f;
            }
        }
    }

    // ★ 추가: BP에서 구현한 발사 이펙트(총구 화염/사운드 등) 호출
    PlayFireEffect();
}



