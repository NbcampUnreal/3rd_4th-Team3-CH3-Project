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
class UHealthComponent; // HealthComponent 전방 선언 (cpp에서 include)


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

    // AK47 무기 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeapon> AK47Class;

    // M16 무기 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeapon> M16Class;

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
    void SwitchToAK47();
    void SwitchToM16();
    void SwitchToPistol();
    void SwitchToShotGun();
    void SwitchToSniperRifle();

    // 무기를 장착
    void EquipWeapon(AWeapon* NewWeapon);
    // 무기 발사
    void Fire();
    // 재장전
    void Reload();
    void InputFire(const FInputActionValue& Value);
    // 조준 시작 / 정지
    void StartAiming();
    void StopAiming();

    UFUNCTION()
    void HandleSelfDied(AActor* DeadActor);

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

    // 애니메이션 몽타주

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AK47ReloadMontage; // AK47 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* M16ReloadMontage; // M16 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* PistolReloadMontage; // Pistol 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* SniperRifleReloadMontage; // SniperRifle 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ShotGunReloadMontage; // Shotgun 재장전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AK47FireHipMontage; // AK47 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AK47FireIronsightsMontage; // AK47 조준 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* M16FireHipMontage; // M16 발사

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* M16FireIronsightsMontage; // M16 조준 발사

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

    // 인터페이스 입력
    UPROPERTY(EditAnywhere, Category = "Input")
    class UInputAction* InteractAction;

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
        // 플레이어 HP 컴포넌트. 캐릭터에 항상 붙어 있게 생성자에서 만들어요.
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
        UHealthComponent* HealthComp = nullptr;   
};
