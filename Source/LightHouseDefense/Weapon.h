// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "E_WeaponType.h"
#include "Weapon.generated.h"

class USceneComponent;
class AWeaponProjectile;
class APawn;
class UTextBlock;                           // [ADD] UI 텍스트 전방 선언

// ===== UI/HUD 통신용 델리게이트들 =====
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, Current, int32, Reserve);   // [ADD]
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadChanged, bool, bReloading);                // [ADD]
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCanFireChanged, bool, bCanFireNow);              // [ADD]

UCLASS(Blueprintable, BlueprintType)
class LIGHTHOUSEDEFENSE_API AWeapon : public AActor
{
    GENERATED_BODY()

public:
    AWeapon();

    // 위젯이 자기 TextBlock을 넘겨줄 때 한 번만 호출
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetAmmoTextBlock(UTextBlock* InText);                 // [ADD]

    // 발사/재장전 호출(BP/코드)
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Fire();

    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    virtual void Reload();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    E_WeaponType WeaponType = E_WeaponType::Pistol;

    // 이펙트/사운드: 무기 BP에서 구현
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void PlayFireEffect();

    // 빈 탄창 트리거(딸깍) – 무기 BP에서 처리
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Ammo")
    void OnDryFire(); // [ADD]

    // 장착/해제
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Equip(APawn* NewOwnerPawn, FName SocketName = "hand_r_socket");

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Unequip();

    /* =========================
       [ADD] UI 바인딩용 이벤트 디스패처
       ========================= */
public: // [ADD]
    UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
    FOnAmmoChanged OnAmmoChanged;           // [ADD]

    UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
    FOnReloadChanged OnReloadChanged;       // [ADD]

    UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
    FOnCanFireChanged OnCanFireChanged;     // [ADD]

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 실제 텍스트 갱신 (C++에서만 호출)
    void UpdateAmmoUI();                    // [ADD]

    // ===== 스탯 =====
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AWeaponProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 25.f; // 샷건의 경우 펠릿 1발 데미지 권장

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Range = 20000.f; // [MOD] 기본 사거리 넉넉히

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate = 10.f; // 초당 발사 속도

    // 샷건 옵션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Shotgun")
    int32 PelletCount = 1; // [ADD] 1이면 단발, >1이면 다중 히트스캔

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Shotgun", meta = (ClampMin = "0.0"))
    float SpreadAngle = 0.f; // [ADD]

    // 자동 재장전 옵션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    bool bAutoReload = false; // [ADD]

    // 총구 컴포넌트(이펙트/라인 시작점)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* Muzzle = nullptr;

    // 장착 소켓
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

    // 내부 처리
    void StartFireCooldown();
    void FinishReload();

private:
    // 위젯의 텍스트블록 레퍼런스
    UPROPERTY() TWeakObjectPtr<UTextBlock> AmmoTextBlock;      // [ADD]
};
