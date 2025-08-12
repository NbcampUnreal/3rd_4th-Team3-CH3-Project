#pragma once

#include "LightHouseDefense/LightHouseDefense.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LightHouseDefense/Weapon.h"
#include "E_WeaponType.h"
#include "CHCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UAnimMontage;

UCLASS()
class LIGHTHOUSEDEFENSE_API ACHCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACHCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;

    // 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComp;

    // 이동
    float NormalSpeed;
    float SprintSpeedMultiplier;
    float SprintSpeed;

    // 조준
    float DefaultFOV;
    float AimingFOV;
    float ZoomInterpSpeed;

    bool bIsSprinting;

    // 지금 장착 무기
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    AWeapon* CurrentWeapon;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeapon> DefaultWeaponClass;

    // AssasultRifle 무기 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeapon> AssasultRifleClass;

    // AssasultRifle2 무기 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeapon> AssasultRifle2Class;

    // 샷건 무기 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeapon> ShotgunClass;

    // 권총 무기 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeapon> PistolClass;

    // 저격 무기 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeapon> SniperRifleClass;

    // 무기 교체 함수들
    void SwitchToAssasultRifle();
    void SwitchToAssasultRifle2();
    void SwitchToPistol();
    void SwitchToShotGun();
    void SwitchToSniperRifle();

    // 무기를 장착
    void EquipWeapon(AWeapon* NewWeapon);
    // 무기 발사
    void Fire();
    // 재장전
    void Reload();
    // 조준 시작 / 정지
    void StartAiming();
    void StopAiming();

    // 조준
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsAiming = false;

    // 재장전
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsReloading = false;


    // 입력 처리 함수들
    UFUNCTION()
    void Move(const FInputActionValue& value);
    UFUNCTION()
    void Look(const FInputActionValue& value);
    UFUNCTION()
    void StartJump(const FInputActionValue& value);
    UFUNCTION()
    void StopJump(const FInputActionValue& value);
    UFUNCTION()
    void StartSprint(const FInputActionValue& value);
    UFUNCTION()
    void StopSprint(const FInputActionValue& value);
    UFUNCTION()
    void StartCrouch(const FInputActionValue& value);
    UFUNCTION()
    void StopCrouch(const FInputActionValue& value);
    UFUNCTION()
    void InputReload(const FInputActionValue& value);
    UFUNCTION()
    void InputFire(const FInputActionValue& value);

    // 애니메이션 몽타주

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AssasultRifleReloadMontage; // AssasultRifle 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AssasultRifle2ReloadMontage; // AssasultRifle2 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* PistolReloadMontage; // Pistol 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* SniperRifleReloadMontage; // SniperRifle 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ShotGunReloadMontage; // Shotgun 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AssasultRifleFireHipMontage; // AssasultRifle 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AssasultRifleFireIronsightsMontage; // AssasultRifle 조준 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AssasultRifle2FireHipMontage; // AssasultRifle2 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AssasultRifle2FireIronsightsMontage; // AssasultRifle2 조준 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* PistolFireHipMontage; // Pistol 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* SniperRifleFireHipMontage; // SniperRifle 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* SniperRifleFireIronsightsMontage; // SniperRifle 조준 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ShotgunFireHipMontage; // 샷건 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ShotgunFireIronsightsMontage; // 샷건 조준 발사

    // 지금 무기 타입 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    E_WeaponType CurrentWeaponType;
};
