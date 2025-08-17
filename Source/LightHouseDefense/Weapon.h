// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USceneComponent;
class AWeaponProjectile;
class APawn;

UCLASS()
class LIGHTHOUSEDEFENSE_API AWeapon : public AActor
{
    GENERATED_BODY()

public:
    AWeapon();

    // 발사 (BP/코드 호출)
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Fire();

    // 재장전
    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    virtual void Reload();

    // BP에서 총구 이펙트/사운드 재생
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void PlayFireEffect();

    // [ADD] 빈 탄창에서 방아쇠 당겼을 때(딸깍) BP에서 처리(사운드/알림 등)
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Ammo")
    void OnDryFire(); // [ADD]

    // 장착/해제
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Equip(APawn* NewOwnerPawn, FName SocketName = "hand_r_socket");
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Unequip();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ===== 스탯 =====
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AWeaponProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 25.f; // 샷건의 경우 펠릿 1발 데미지로 세팅 권장

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Range = 20000.f; // [MOD] BP에서 총별 사거리 조절 (기본 넉넉히) 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate = 10.f; // 초당 발사 속도(쿨다운 계산에 사용)

    // [ADD] 샷건 퍼짐: 1이면 일반 총기, >1이면 샷건(다중 히트스캔)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Shotgun")
    int32 PelletCount = 1; // 샷건: 6~12 추천

    // [ADD] 산탄 각도(도 단위): Pitch/Yaw 각각 ±SpreadAngle 범위에서 랜덤
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Shotgun", meta = (ClampMin = "0.0"))
    float SpreadAngle = 0.f; // 샷건: 3~6도 추천

    // [ADD] 자동 재장전 여부(기본 꺼짐 → 수동 재장전만 허용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    bool bAutoReload = false; // [ADD]

    // 총구 컴포넌트(이펙트/총알 시작점)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* Muzzle = nullptr;

    // 장착된 소켓 이름
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    FName EquippedSocketName = NAME_None;

    // ===== 탄약 =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    int32 MagazineSize = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    int32 CurrentAmmo = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    int32 ReserveAmmo = 90;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    float ReloadTime = 1.8f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    bool bIsReloading = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
    bool bCanFire = true;

    // 타이머
    FTimerHandle TH_FireCooldown;
    FTimerHandle TH_Reload;

    // 재장전 BP 이벤트(애니/사운드용)
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Ammo")
    void OnReloadStarted();
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Ammo")
    void OnReloadFinished();

    // 내부
    void StartFireCooldown();
    void FinishReload();
};







