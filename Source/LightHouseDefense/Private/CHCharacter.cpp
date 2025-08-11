#include "CHCharacter.h"
#include "CHPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ACHCharacter::ACHCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentWeapon = EWeaponType::Rifle;

    // 스프링암 설정
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(GetMesh(), TEXT("Head"));
    SpringArmComp->TargetArmLength = 0.f;
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->bDoCollisionTest = false;

    // 카메라 설정
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    // 이동 속도 설정
    NormalSpeed = 600.0f;
    SprintSpeedMultiplier = 1.5f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;


    DefaultFOV = 90.0f;      // 기본 시야
    AimingFOV = 60.0f;       // 조준 시 시야
    ZoomInterpSpeed = 20.0f; // 부드럽게 이동하는 속도
}

void ACHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {

        if (ACHPlayerController* PlayerController = Cast<ACHPlayerController>(GetController()))
        {
            // 기본 이동 입력
            if (PlayerController->MoveAction)
            {
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ACHCharacter::Move);
            }
            if (PlayerController->LookAction)
            {
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ACHCharacter::Look);
            }
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ACHCharacter::StartJump);
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ACHCharacter::StopJump);
            }
            if (PlayerController->SprintAction)
            {
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &ACHCharacter::StartSprint);
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &ACHCharacter::StopSprint);
            }
            if (PlayerController->CrouchAction)
            {
                EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Triggered, this, &ACHCharacter::StartCrouch);
                EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Completed, this, &ACHCharacter::StopCrouch);
            }
            if (PlayerController->RifleAction)
            {
                EnhancedInput->BindAction(PlayerController->RifleAction, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToRifle);
            }
            if (PlayerController->ShotgunAction)
            {
                EnhancedInput->BindAction(PlayerController->ShotgunAction, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToShotgun);
            }

            // 공통 무기 기능
            if (PlayerController->FireAction)
            {
                EnhancedInput->BindAction(PlayerController->FireAction, ETriggerEvent::Triggered, this, &ACHCharacter::Fire);
            }


            if (PlayerController->AimAction)
            {
                EnhancedInput->BindAction(PlayerController->AimAction, ETriggerEvent::Started, this, &ACHCharacter::StartAiming);
                EnhancedInput->BindAction(PlayerController->AimAction, ETriggerEvent::Completed, this, &ACHCharacter::StopAiming);
            }

            if (PlayerController->ReloadAction)
            {
                EnhancedInput->BindAction(PlayerController->ReloadAction, ETriggerEvent::Triggered, this, &ACHCharacter::Reload);
            }
        }
    }
}


void ACHCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (CameraComp)
    {
        DefaultFOV = CameraComp->FieldOfView;
    }
}

void ACHCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CameraComp)
    {
        float TargetFOV = bIsAiming ? AimingFOV : DefaultFOV;
        float CurrentFOV = CameraComp->FieldOfView;
        float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, ZoomInterpSpeed);
        CameraComp->SetFieldOfView(NewFOV);
    }
}

void ACHCharacter::SwitchWeapon(EWeaponType NewWeapon)
{
    UE_LOG(LogTemp, Warning, TEXT("SwitchWeapon!"));

}


void ACHCharacter::StartAiming()
{
    bIsAiming = true;
}

void ACHCharacter::StopAiming()
{
    bIsAiming = false;
}

void ACHCharacter::Reload()
{
    UE_LOG(LogTemp, Warning, TEXT("Reload"));

    if (RifleReloadMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && !AnimInstance->Montage_IsPlaying(RifleReloadMontage))
        {
            AnimInstance->Montage_Play(RifleReloadMontage);
        }
    }

    if (ShotGunReloadMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && !AnimInstance->Montage_IsPlaying(ShotGunReloadMontage))
        {
            AnimInstance->Montage_Play(ShotGunReloadMontage);
        }
    }
}
void ACHCharacter::InputReload(const FInputActionValue& Value)
{
    Reload();
}


// 입력 함수들
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

void ACHCharacter::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();
    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
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

void ACHCharacter::StartSprint(const FInputActionValue& Value)
{
    bIsSprinting = true;
}

void ACHCharacter::StopSprint(const FInputActionValue& Value)
{
    bIsSprinting = false;
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

void ACHCharacter::SwitchToRifle()
{
}

void ACHCharacter::SwitchToShotgun()
{
}

void ACHCharacter::Fire()
{
    UE_LOG(LogTemp, Warning, TEXT("Fire"));

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        UAnimMontage* CurrentFireMontage = nullptr;

        // 무기에 따라 몽타주를 선택
        if (CurrentWeapon == EWeaponType::Rifle)
        {
            // 조준 상태에 따라 몽타주를 선택
            if (bIsAiming)
            {
                CurrentFireMontage = RifleFireIronsightsMontage;
            }
            else // 조준하지 않은 상태
            {
                CurrentFireMontage = RifleFireHipMontage;
            }
        }
        else if (CurrentWeapon == EWeaponType::Shotgun)
        {
            if (bIsAiming)
            {
                CurrentFireMontage = ShotgunFireIronsightsMontage;
            }
            else
            {
                CurrentFireMontage = ShotgunFireHipMontage;
            }
        }

        // 선택된 몽타주가 유효한지 확인하고 재생
        if (CurrentFireMontage && !AnimInstance->Montage_IsPlaying(CurrentFireMontage))
        {
            AnimInstance->Montage_Play(CurrentFireMontage);

            bIsFire = true;
        }
    }
}

