#include "CHCharacter.h"
#include "CHPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACHCharacter::ACHCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentWeapon = EWeaponType::Rifle;

	// 스프링암 생성 및 설정
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(GetMesh(), TEXT("Head"));
	SpringArmComp->TargetArmLength = 0.f; // 카메라가 캐릭터에 딱 붙게 설정
	SpringArmComp->bUsePawnControlRotation = true; // 캐릭터가 회전할 때 암도 같이 회전
	SpringArmComp->bDoCollisionTest = false;

	// 카메라 생성 및 설정
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false; // 카메라는 직접 회전하지 않고 스프링암 따라감

	// 이동 속도 설정
	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void ACHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ACHPlayerController* PlayerController = Cast<ACHPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ACHCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ACHCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ACHCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ACHCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ACHCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ACHCharacter::StopSprint
				);
			}

			if (PlayerController->CrouchAction)
			{
				EnhancedInput->BindAction(
					PlayerController->CrouchAction,
					ETriggerEvent::Triggered,
					this,
					&ACHCharacter::StartCrouch
				);

				EnhancedInput->BindAction(
					PlayerController->CrouchAction,
					ETriggerEvent::Completed,
					this,
					&ACHCharacter::StopCrouch
				);
			}
			if (PlayerController->RifleAction)
			{
				EnhancedInput->BindAction(
					PlayerController->RifleAction,
					ETriggerEvent::Triggered,
					this,
					&ACHCharacter::SwitchToRifle
				);
			}

			if (PlayerController->ShotgunAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ShotgunAction,
					ETriggerEvent::Triggered,
					this,
					&ACHCharacter::SwitchToShotgun
				);
			}

			if (PlayerController->PistolAction)
			{
				EnhancedInput->BindAction(
					PlayerController->PistolAction,
					ETriggerEvent::Triggered,
					this,
					&ACHCharacter::SwitchToPistol
				);
			}
		}
	}
}

void ACHCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ACHCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void ACHCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void ACHCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ACHCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ACHCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void ACHCharacter::StartCrouch(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Crouch();
	}
}

void ACHCharacter::StopCrouch(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		UnCrouch();
	}
}

void ACHCharacter::SwitchWeapon(EWeaponType NewWeapon)
{
	CurrentWeapon = NewWeapon;
	UE_LOG(LogTemp, Warning, TEXT("Weapon switched to %d"), (int32)NewWeapon);
}

void ACHCharacter::SwitchToRifle()
{
	SwitchWeapon(EWeaponType::Rifle);
}

void ACHCharacter::SwitchToShotgun()
{
	SwitchWeapon(EWeaponType::Shotgun);
}

void ACHCharacter::SwitchToPistol()
{
	SwitchWeapon(EWeaponType::Pistol);
}

