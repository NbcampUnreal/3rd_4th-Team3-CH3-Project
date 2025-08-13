#include "CHCharacter.h"
#include "CHPlayerController.h"
#include "LightHouseDefense/Weapon.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "E_WeaponType.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "HealthComponent.h"      // HealthComp 생성/사용
#include "HealthSubsystem.h"      // 초기 HP 세팅 호출
#include "Engine/GameInstance.h"  // GetGameInstance()

ACHCharacter::ACHCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentWeaponType = E_WeaponType::AK47;
    CurrentWeapon = nullptr;

    // 스프링암 설정
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(GetMesh(), FName("Head"));
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

    // ========================= [ADDED] =========================
    // 플레이어에 HealthComponent를 "항상" 붙여둠 (BeginPlay에서 자동으로 AnyDamage에 바인딩됨)
    HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    // ===========================================================

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



    // HealthSubsystem을 통해 초기 HP/팀 세팅 (편의상 Subsystem이 기본값을 관리)
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UHealthSubsystem* HS = GI->GetSubsystem<UHealthSubsystem>())
        {
            HS->InitializeHealthForActor(this);
            // 필요 시 여기서도 초기값을 직접 찍어볼 수 있음:
            // UE_LOG(LogTemp, Log, TEXT("[CHCharacter] HealthComp ready: %.0f/%.0f"),
            //     HealthComp ? HealthComp->GetHealth() : -1.f,
            //     HealthComp ? HealthComp->GetMaxHealth() : -1.f);
        }
    }

    // 사망 이벤트를 "캐릭터 자신"도 수신 → 즉시 이동/입력 차단
    if (UHealthComponent* HC = FindComponentByClass<UHealthComponent>())
    {
        HC->OnDied.AddDynamic(this, &ACHCharacter::HandleSelfDied);
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
            if (PlayerController->AK47Action)
            {
                EnhancedInput->BindAction(PlayerController->AK47Action, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToAK47);
            }
            if (PlayerController->M16Action)
            {
                EnhancedInput->BindAction(PlayerController->M16Action, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToM16);
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

void ACHCharacter::SwitchToAK47()
{
    if (AK47Class)
    {
        if (CurrentWeapon) CurrentWeapon->Destroy();
        AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(AK47Class);
        EquipWeapon(NewWeapon);
    }
}

void ACHCharacter::SwitchToM16()
{
    if (M16Class)
    {
        if (CurrentWeapon) CurrentWeapon->Destroy();
        AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(M16Class);
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

void ACHCharacter::HandleSelfDied(AActor* DeadActor)
{
    if (DeadActor != this) return;

    // 이동 완전 정지
    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->StopMovementImmediately();
        Move->DisableMovement();
    }

    // 입력 차단
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Player] Died -> movement/input disabled"));
}
