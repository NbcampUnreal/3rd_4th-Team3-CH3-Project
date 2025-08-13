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

    // 발사
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Fire();

    // 재장전
    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    virtual void Reload(); 

    // BP에서 총구 화염/사운드
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void PlayFireEffect();

    // 장착/해제
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Equip(APawn* NewOwnerPawn, FName SocketName = "hand_r_socket");

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Unequip();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ===== 발사체 / 기본 스탯 =====
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AWeaponProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 25.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Range = 1000.f;

    // 초당 발사수(예: 10 → 0.1초 간격)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate = 10.f; 

    // 총구 컴포넌트 (스폰 위치)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* Muzzle = nullptr;

    // 장착 소켓 캐시
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    FName EquippedSocketName = NAME_None;

    // ===== 탄약/재장전 =====
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

    // 타이머 핸들
    FTimerHandle TH_FireCooldown; 
    FTimerHandle TH_Reload;       

    // 재장전 BP 훅 (옵션)
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Ammo")
    void OnReloadStarted(); 

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Ammo")
    void OnReloadFinished(); 

    // 내부 헬퍼
    void StartFireCooldown(); 
    void FinishReload();      
};



