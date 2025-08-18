#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "E_WeaponType.h"                
#include "CHCharacter.generated.h"       

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UAnimMontage;
class UHealthComponent;
class UInputAction;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquippedSig, E_WeaponType, NewType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponHolsteredSig);

UCLASS()
class LIGHTHOUSEDEFENSE_API ACHCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACHCharacter();

    // 현재 장착 무기 (HUD/블루프린트에서 쓰기 좋게 제공)
    UFUNCTION(BlueprintPure, Category = "Weapon")
    AWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    E_WeaponType GetCurrentWeaponType() const { return CurrentWeaponType; }

    // BP에서 “이벤트 추가”로 받을 수 있는 바인더블 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
    FOnWeaponEquippedSig OnWeaponEquipped;

    UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
    FOnWeaponHolsteredSig OnWeaponHolstered;

    // BP가 ‘오버라이드(Implement)’해서 애니/사운드 틀 수 있는 훅
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|AnimSFX", meta = (DisplayName = "OnWeaponEquipped"))
    void BP_OnWeaponEquipped(E_WeaponType NewType);

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|AnimSFX", meta = (DisplayName = "OnWeaponHolstered"))
    void BP_OnWeaponHolstered();

    // 토글 한 방에 처리(제네릭)
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ToggleWeapon(E_WeaponType Type);

    // 무기별 토글 래퍼
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ToggleAK47();
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ToggleM16();
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void TogglePistol();
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ToggleShotgun();
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ToggleSniperRifle();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;

    // ---------- 카메라 ----------
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComp;

    // 카메라 상하 각도 제한
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraMinPitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraMaxPitch;

    // ---------- 이동 ----------
    float NormalSpeed;
    float SprintSpeedMultiplier;
    float SprintSpeed;

    // ---------- FOV/줌 ----------
    float DefaultFOV;
    float AimingFOV;
    float ZoomInterpSpeed;

    bool bIsSprinting;

    // ---------- 무기 ----------
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    AWeapon* CurrentWeapon;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToHand();

    // 시작/전환에 사용할 무기 클래스들
    UPROPERTY(EditDefaultsOnly, Category = "Weapon") TSubclassOf<AWeapon> DefaultWeaponClass;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon") TSubclassOf<AWeapon> AK47Class;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon") TSubclassOf<AWeapon> M16Class;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon") TSubclassOf<AWeapon> ShotgunClass;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon") TSubclassOf<AWeapon> PistolClass;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon") TSubclassOf<AWeapon> SniperRifleClass;

    // 인벤토리: 스폰한 무기를 보관(파괴 X → 탄 수 유지)
    UPROPERTY() TMap<E_WeaponType, AWeapon*> WeaponInventory;

    // 무기 교체
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToAK47();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToM16();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToPistol();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToShotGun();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToSniperRifle();

    // 내부 유틸
    void EquipWeapon(AWeapon* NewWeapon);
    AWeapon* GetOrSpawnWeapon(E_WeaponType Type);
    TSubclassOf<AWeapon> GetClassByType(E_WeaponType Type) const;

    // ---------- 입력/행동 ----------
    void Fire();
    void InputFire(const FInputActionValue& Value);
    void Reload();
    void InputReload(const FInputActionValue& Value);

    void StartAiming();
    void StopAiming();

    UFUNCTION()
    void HandleSelfDied(AActor* DeadActor);

    // ---------- 상태 ----------
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsAiming = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsReloading = false;

    // ---------- 입력 처리 ----------
    UFUNCTION() void Move(const FInputActionValue& value);
    UFUNCTION() void Look(const FInputActionValue& value);
    UFUNCTION() void StartJump(const FInputActionValue& value);
    UFUNCTION() void StopJump(const FInputActionValue& value);
    UFUNCTION() void StartSprint(const FInputActionValue& value);
    UFUNCTION() void StopSprint(const FInputActionValue& value);
    UFUNCTION() void StartCrouch(const FInputActionValue& value);
    UFUNCTION() void StopCrouch(const FInputActionValue& value);

    // ---------- 애니메이션 ----------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* AK47ReloadMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* M16ReloadMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* PistolReloadMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* SniperRifleReloadMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* ShotGunReloadMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* AK47FireHipMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* AK47FireIronsightsMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* M16FireHipMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* M16FireIronsightsMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* PistolFireHipMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* SniperRifleFireHipMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* SniperRifleFireIronsightsMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* ShotgunFireHipMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimMontage* ShotgunFireIronsightsMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
    UAnimMontage* DeathMontage;

    // 현재 무기 타입
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    E_WeaponType CurrentWeaponType;

    // 인터페이스 입력
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* InteractAction;

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

private:
    // 체력 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
    UHealthComponent* HealthComp = nullptr;

    // 전환을 하나로 합친 범용 함수
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToWeapon(E_WeaponType Type);
};
