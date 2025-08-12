#include "CHCharacter.h"
#include "CHPlayerController.h"
#include "LightHouseDefense/Weapon.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "E_WeaponType.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

ACHCharacter::ACHCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentWeaponType = E_WeaponType::AssasultRifle;
    CurrentWeapon = nullptr;

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

    // 카메라 시야
    DefaultFOV = 90.0f;
    AimingFOV = 60.0f;
    ZoomInterpSpeed = 20.0f;

    // 상태 변수 초기화
    bIsAiming = false;
    bIsSprinting = false;
    bIsReloading = false;
}

void ACHCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (CameraComp)
    {
        DefaultFOV = CameraComp->FieldOfView;
    }

    if (DefaultWeaponClass)
    {
        AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
        if (SpawnedWeapon)
        {
            EquipWeapon(SpawnedWeapon);
        }
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

void ACHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {

        if (ACHPlayerController* PlayerController = Cast<ACHPlayerController>(GetController()))
        {
            // 이동/시점
            if (PlayerController->MoveAction)
            {
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ACHCharacter::Move);
            }
            if (PlayerController->LookAction)
            {
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ACHCharacter::Look);
            }

            // 점프
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ACHCharacter::StartJump);
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ACHCharacter::StopJump);
            }

            // 달리기
            if (PlayerController->SprintAction)
            {
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &ACHCharacter::StartSprint);
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &ACHCharacter::StopSprint);
            }

            // 앉기
            if (PlayerController->CrouchAction)
            {
                EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Triggered, this, &ACHCharacter::StartCrouch);
                EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Completed, this, &ACHCharacter::StopCrouch);
            }

            // 무기
            if (PlayerController->AssasultRifleAction)
            {
                EnhancedInput->BindAction(PlayerController->AssasultRifleAction, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToAssasultRifle);
            }
            if (PlayerController->AssasultRifle2Action)
            {
                EnhancedInput->BindAction(PlayerController->AssasultRifle2Action, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToAssasultRifle2);
            }
            if (PlayerController->PistolAction)
            {
                EnhancedInput->BindAction(PlayerController->PistolAction, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToPistol);
            }
            if (PlayerController->SinperRifleAction)
            {
                EnhancedInput->BindAction(PlayerController->SinperRifleAction, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToSniperRifle);
            }
            if (PlayerController->ShotgunAction)
            {
                EnhancedInput->BindAction(PlayerController->ShotgunAction, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToShotGun);
            }

            // 발사 / 재장전
            if (PlayerController->FireAction)
            {
                EnhancedInput->BindAction(PlayerController->FireAction, ETriggerEvent::Triggered, this, &ACHCharacter::Fire);
            }
            if (PlayerController->ReloadAction)
            {
                EnhancedInput->BindAction(PlayerController->ReloadAction, ETriggerEvent::Triggered, this, &ACHCharacter::Reload);
            }

            // 조준
            if (PlayerController->AimAction)
            {
                EnhancedInput->BindAction(PlayerController->AimAction, ETriggerEvent::Started, this, &ACHCharacter::StartAiming);
                EnhancedInput->BindAction(PlayerController->AimAction, ETriggerEvent::Completed, this, &ACHCharacter::StopAiming);
            }

            
        }
    }
}

// 무기 장착
void ACHCharacter::EquipWeapon(AWeapon* NewWeapon)
{
    if (!NewWeapon) return;

    if (CurrentWeapon)
    {
        CurrentWeapon->Destroy();   // 이전 무기 제거
    }

    CurrentWeapon = NewWeapon;
    CurrentWeapon->Equip(this, TEXT("WeaponSocket"));
}

void ACHCharacter::Fire()
{
}

// 재장전 입력
void ACHCharacter::Reload()
{
    if (CurrentWeapon)
    {
//      CurrentWeapon->Reload();
    }
}

void ACHCharacter::InputFire(const FInputActionValue& Value)
{
    Fire();
}

void ACHCharacter::InputReload(const FInputActionValue& Value)
{
    Reload();
}

// 이동
void ACHCharacter::Move(const FInputActionValue& value)
{
    if (!Controller) return;

    const FVector2D MoveInput = value.Get<FVector2D>();
    // 앞뒤 이동
    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
    }
    // 좌우 이동
    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
    }
}

// 시점
void ACHCharacter::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();
    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
}

// 점프
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

// 달리기
void ACHCharacter::StartSprint(const FInputActionValue& Value)
{
    bIsSprinting = true;
}
void ACHCharacter::StopSprint(const FInputActionValue& Value)
{
    bIsSprinting = false;
}

// 앉기
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

void ACHCharacter::SwitchToAssasultRifle()
{
    if (AssasultRifleClass)
    {
        if (CurrentWeapon) CurrentWeapon->Destroy();
        AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(AssasultRifleClass);
        EquipWeapon(NewWeapon);
    }
}

void ACHCharacter::SwitchToAssasultRifle2()
{
    if (AssasultRifle2Class)
    {
        if (CurrentWeapon) CurrentWeapon->Destroy();
        AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(AssasultRifle2Class);
        EquipWeapon(NewWeapon);
    }
}

void ACHCharacter::SwitchToPistol()
{
    if (PistolClass)
    {
        if (CurrentWeapon) CurrentWeapon->Destroy();
        AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(PistolClass);
        EquipWeapon(NewWeapon);
    }
}

void ACHCharacter::SwitchToShotGun()
{
    if (ShotgunClass)
    {
        if (CurrentWeapon) CurrentWeapon->Destroy();
        AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(ShotgunClass);
        EquipWeapon(NewWeapon);
    }
}

void ACHCharacter::SwitchToSniperRifle()
{
    if (SniperRifleClass)
    {
        if (CurrentWeapon) CurrentWeapon->Destroy();
        AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(SniperRifleClass);
        EquipWeapon(NewWeapon);
    }
}

void ACHCharacter::StartAiming()
{
    bIsAiming = true;
}

void ACHCharacter::StopAiming()
{
    bIsAiming = false;
}
