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

    //  무기 발사 함수 (BP/코드 호출 가능)
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Fire();

    //  재장전 함수
    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    virtual void Reload();

    //  BP에서 총구 화염/사운드 재생 (이벤트)
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void PlayFireEffect();

    //  무기 장착
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Equip(APawn* NewOwnerPawn, FName SocketName = "hand_r_socket");

    //  무기 해제
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Unequip();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ===== 무기 스탯 =====
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AWeaponProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 25.f; //  데미지량

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Range = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate = 10.f; //  초당 발사 속도

    // 총구 위치
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* Muzzle = nullptr;

    // 장착된 소켓 이름
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    FName EquippedSocketName = NAME_None;

    // ===== 탄약/재장전 =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    int32 MagazineSize = 30;  // 탄창 크기

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    int32 CurrentAmmo = 30;   // 현재 탄약

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    int32 ReserveAmmo = 90;   // 예비 탄약

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    float ReloadTime = 1.8f;  // 재장전 시간

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    bool bIsReloading = false; // 재장전 중 여부

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
    bool bCanFire = true;      // 발사 가능 여부

    // 타이머 핸들
    FTimerHandle TH_FireCooldown;
    FTimerHandle TH_Reload;

    // 재장전 시작/완료 BP 이벤트
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Ammo")
    void OnReloadStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Ammo")
    void OnReloadFinished();

    // 내부 함수
    void StartFireCooldown();
    void FinishReload();
};



