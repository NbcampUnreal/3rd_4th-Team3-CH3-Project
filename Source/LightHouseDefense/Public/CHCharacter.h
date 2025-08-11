#pragma once

#include "LightHouseDefense/LightHouseDefense.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CHWeaponEnums.h"
#include "CHCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class LIGHTHOUSEDEFENSE_API ACHCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACHCharacter();

    float DefaultFOV;
    float AimingFOV;
    float ZoomInterpSpeed;

protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
    void SwitchToRifle();
    UFUNCTION()
    void SwitchToShotgun();
    UFUNCTION()
    void InputReload(const FInputActionValue& Value);


    // 무기 관련 기능
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchWeapon(EWeaponType NewWeapon);
    void StartAiming();
    void StopAiming();
    void Fire();
    void Reload();


protected:
    // 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComp;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 이동 속도
    float NormalSpeed;
    float SprintSpeedMultiplier;
    float SprintSpeed;

    // 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsAiming = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsReloading = false;

    // 무기 상태
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    EWeaponType CurrentWeapon;


    // 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* RifleReloadMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ShotGunReloadMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* RifleFireHipMontage; // 소총 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* RifleFireIronsightsMontage; // 소총 조준 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ShotgunFireHipMontage; // 샷건 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ShotgunFireIronsightsMontage; // 샷건 조준 발사

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsFire;

    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    bool bIsSprinting = false;
};
