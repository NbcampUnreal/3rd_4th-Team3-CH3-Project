#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CHWeaponEnums.h"
#include "CHCharacter.generated.h"



class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class CH_API ACHCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACHCharacter();

	//카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Carema")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Carema")
	UCameraComponent* CameraComp;

	//무기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType CurrentWeapon;
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchWeapon(EWeaponType NewWeapon);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
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
	void SwitchToPistol();

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
};


