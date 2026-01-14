#include "CHCharacter.h"
#include "CHPlayerController.h"
#include "LighthouseHUD.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "E_WeaponType.h"
#include "../Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "HealthComponent.h"
#include "HealthSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

ACHCharacter::ACHCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentWeaponType = E_WeaponType::AK47;
    CurrentWeapon = nullptr;

    AK47FireHipMontage = nullptr;
    AK47FireIronsightsMontage = nullptr;
    AK47ReloadMontage = nullptr;
    DeathMontage = nullptr;
    InteractAction = nullptr;
    M16FireHipMontage = nullptr;
    M16FireIronsightsMontage = nullptr;
    M16ReloadMontage = nullptr;
    PistolFireHipMontage = nullptr;
    PistolReloadMontage = nullptr;
    ShotgunFireHipMontage = nullptr;
    ShotgunFireIronsightsMontage = nullptr;
    ShotGunReloadMontage = nullptr;
    SniperRifleFireHipMontage = nullptr;
    SniperRifleFireIronsightsMontage = nullptr;
    SniperRifleReloadMontage = nullptr;

    // 스프링암
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(GetMesh(), FName("Head"));
    SpringArmComp->TargetArmLength = 0.f;
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->bDoCollisionTest = false;

    // 카메라
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    // 이동
    NormalSpeed = 600.0f;
    SprintSpeedMultiplier = 1.5f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

    // FOV
    DefaultFOV = 90.0f;
    AimingFOV = 60.0f;
    ZoomInterpSpeed = 20.0f;

    CameraMinPitch = -30.0f;
    CameraMaxPitch = 30.0f;

    // 체력 컴포넌트
    HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

    // 상태
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

    // 초기 HP/팀 세팅
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UHealthSubsystem* HS = GI->GetSubsystem<UHealthSubsystem>())
        {
            HS->InitializeHealthForActor(this);
        }
    }

    if (UHealthComponent* HC = FindComponentByClass<UHealthComponent>())
    {
        HC->OnDied.AddDynamic(this, &ACHCharacter::HandleSelfDied);
    }

    // 기본 무기: 스폰 후 장착(파괴하지 않고 인벤토리에 남김)
    if (DefaultWeaponClass)
    {
        AWeapon* Spawned = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
        if (Spawned)
        {
            WeaponInventory.Add(CurrentWeaponType, Spawned);
            EquipWeapon(Spawned); // HUD 재바인딩 포함
        }
    }

    SwitchToPistol();
}

void ACHCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CameraComp)
    {
        const float TargetFOV = bIsAiming ? AimingFOV : DefaultFOV;
        const float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
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
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ACHCharacter::Move);
            if (PlayerController->LookAction)
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ACHCharacter::Look);

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

            // 무기 전환
            if (PlayerController->AK47Action)        EnhancedInput->BindAction(PlayerController->AK47Action, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToAK47);
            if (PlayerController->M16Action)         EnhancedInput->BindAction(PlayerController->M16Action, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToM16);
            if (PlayerController->PistolAction)      EnhancedInput->BindAction(PlayerController->PistolAction, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToPistol);
            if (PlayerController->SinperRifleAction) EnhancedInput->BindAction(PlayerController->SinperRifleAction, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToSniperRifle);
            if (PlayerController->ShotgunAction)     EnhancedInput->BindAction(PlayerController->ShotgunAction, ETriggerEvent::Triggered, this, &ACHCharacter::SwitchToShotGun);

            // 발사/재장전
            if (PlayerController->FireAction)
                EnhancedInput->BindAction(PlayerController->FireAction, ETriggerEvent::Triggered, this, &ACHCharacter::Fire);
            if (PlayerController->ReloadAction)
                EnhancedInput->BindAction(PlayerController->ReloadAction, ETriggerEvent::Triggered, this, &ACHCharacter::Reload);

            // 조준
            if (PlayerController->AimAction)
            {
                EnhancedInput->BindAction(PlayerController->AimAction, ETriggerEvent::Started, this, &ACHCharacter::StartAiming);
                EnhancedInput->BindAction(PlayerController->AimAction, ETriggerEvent::Completed, this, &ACHCharacter::StopAiming);
            }
        }
    }
}

/* ====================== 인벤토리/장착 로직 ====================== */

TSubclassOf<AWeapon> ACHCharacter::GetClassByType(E_WeaponType Type) const
{
    switch (Type)
    {
    case E_WeaponType::AK47:        return AK47Class;
    case E_WeaponType::M16:         return M16Class;
    case E_WeaponType::Pistol:      return PistolClass;
    case E_WeaponType::Shotgun:     return ShotgunClass;
    case E_WeaponType::SniperRifle: return SniperRifleClass;
    default:                        return DefaultWeaponClass;
    }
}

AWeapon* ACHCharacter::GetOrSpawnWeapon(E_WeaponType Type)
{
    if (AWeapon** Found = WeaponInventory.Find(Type))
    {
        return *Found; // 이미 인벤토리에 있음
    }

    TSubclassOf<AWeapon> Cls = GetClassByType(Type);
    if (!Cls) return nullptr;

    AWeapon* NewWep = GetWorld()->SpawnActor<AWeapon>(Cls);
    if (NewWep)
    {
        // 미리 붙여두고 숨겨놓음 (탄/설정 유지)
        NewWep->Equip(this, TEXT("WeaponSocket"));
        NewWep->SetActorHiddenInGame(true);
        NewWep->SetActorEnableCollision(false);
        WeaponInventory.Add(Type, NewWep);
    }
    return NewWep;
}

void ACHCharacter::EquipWeapon(AWeapon* NewWeapon)
{
    if (!NewWeapon) return;

    // 이전 무기는 파괴하지 말고 숨겨서 보관
    if (CurrentWeapon)
    {
        CurrentWeapon->SetActorHiddenInGame(true);
        CurrentWeapon->SetActorEnableCollision(false);
    }

    CurrentWeapon = NewWeapon;
    CurrentWeapon->Equip(this, TEXT("WeaponSocket"));
    CurrentWeapon->SetActorHiddenInGame(false);
    CurrentWeapon->SetActorEnableCollision(true);

    // HUD에 현재 무기로 재바인딩 요청 (AmmoText 연결)
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (AHUD* H = PC->GetHUD())
        {
            if (ALighthouseHUD* LH = Cast<ALighthouseHUD>(H))
            {
                LH->TryBindWeaponToAmmoText();
            }
        }
    }
}

/* ====================== 입력 처리 ====================== */

void ACHCharacter::Fire()
{
    // ★ hand/맨손 상태이거나 무기가 없으면 즉시 무시
    if (CurrentWeaponType == E_WeaponType::None || CurrentWeapon == nullptr)
    {
        // UE_LOG(LogTemp, Verbose, TEXT("[Fire] Ignored in hand state"));
        return;
    }

    if (!CurrentWeapon) return;
    if (bIsSprinting || bIsReloading) return;

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (PC->bShowMouseCursor)   // UI 조작 중으로 판단
            return;
    }
    if (CurrentWeapon->GetOwner() != this) return;

    CurrentWeapon->Fire(); // 탄 감소/쿨다운/효과는 무기 쪽에서 처리
}

void ACHCharacter::Reload()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->Reload();
    }
}

void ACHCharacter::InputFire(const FInputActionValue& Value) { Fire(); }
void ACHCharacter::InputReload(const FInputActionValue& Value) { Reload(); }

// 이동/시점
void ACHCharacter::Move(const FInputActionValue& value)
{
    if (!Controller) return;
    const FVector2D M = value.Get<FVector2D>();
    if (!FMath::IsNearlyZero(M.X)) AddMovementInput(GetActorForwardVector(), M.X);
    if (!FMath::IsNearlyZero(M.Y)) AddMovementInput(GetActorRightVector(), M.Y);
}

void ACHCharacter::Look(const FInputActionValue& value)
{
    const FVector2D L = value.Get<FVector2D>();
    AddControllerYawInput(L.X);
    if (!FMath::IsNearlyZero(L.Y))
    {
        if (AController* C = GetController())
        {
            FRotator R = C->GetControlRotation(); R.Normalize();
            float NewPitch = FMath::Clamp(R.Pitch - L.Y, CameraMinPitch, CameraMaxPitch);
            C->SetControlRotation(FRotator(NewPitch, R.Yaw, R.Roll));
        }
    }
}

// 점프
void ACHCharacter::StartJump(const FInputActionValue& v) { if (v.Get<bool>()) Jump(); }
void ACHCharacter::StopJump(const FInputActionValue& v) { if (!v.Get<bool>()) StopJumping(); }

// 달리기/앉기
void ACHCharacter::StartSprint(const FInputActionValue&) { bIsSprinting = true; }
void ACHCharacter::StopSprint(const FInputActionValue&) { bIsSprinting = false; }
void ACHCharacter::StartCrouch(const FInputActionValue& v) { if (v.Get<bool>()) Crouch(); }
void ACHCharacter::StopCrouch(const FInputActionValue& v) { if (!v.Get<bool>()) UnCrouch(); }

/* ====================== 무기 전환 ====================== */
void ACHCharacter::SwitchToAK47()
{
    CurrentWeaponType = E_WeaponType::AK47;
    SwitchToWeapon(E_WeaponType::AK47);
    if (AWeapon* W = GetOrSpawnWeapon(CurrentWeaponType)) EquipWeapon(W);
}
void ACHCharacter::SwitchToM16()
{
    CurrentWeaponType = E_WeaponType::M16;
    SwitchToWeapon(E_WeaponType::M16);
    if (AWeapon* W = GetOrSpawnWeapon(CurrentWeaponType)) EquipWeapon(W);
}
void ACHCharacter::SwitchToPistol()
{
    CurrentWeaponType = E_WeaponType::Pistol;
    SwitchToWeapon(E_WeaponType::Pistol);
    if (AWeapon* W = GetOrSpawnWeapon(CurrentWeaponType)) EquipWeapon(W);
}
void ACHCharacter::SwitchToShotGun()
{
    CurrentWeaponType = E_WeaponType::Shotgun;
    SwitchToWeapon(E_WeaponType::Shotgun);
    if (AWeapon* W = GetOrSpawnWeapon(CurrentWeaponType)) EquipWeapon(W);
}
void ACHCharacter::SwitchToSniperRifle()
{
    CurrentWeaponType = E_WeaponType::SniperRifle;
    SwitchToWeapon(E_WeaponType::SniperRifle);
    if (AWeapon* W = GetOrSpawnWeapon(CurrentWeaponType)) EquipWeapon(W);
}

/* ====================== 기타 ====================== */

void ACHCharacter::StartAiming() { bIsAiming = true; }
void ACHCharacter::StopAiming() { bIsAiming = false; }

void ACHCharacter::HandleSelfDied(AActor* DeadActor)
{
    if (DeadActor != this) return;

    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->StopMovementImmediately();
        Move->DisableMovement();
    }

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
    }

    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
        {
            if (DeathMontage) { Anim->Montage_Play(DeathMontage); }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[Player] Died -> movement/input disabled"));
}

float ACHCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    UE_LOG(LogTemp, Warning, TEXT("take damage"));
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACHCharacter::SwitchToHand()
{
    // 1) 현재 무기가 있으면 HUD 연결 해제 + 화면에서 떼기
    if (CurrentWeapon)
    {
        // HUD 텍스트 블록 연결 끊기 → 이후 UpdateAmmoUI가 와도 텍스트 갱신 안됨
        //CurrentWeapon->SetAmmoTextBlock(nullptr);

        // 폰에서 분리 (AttachedActors로 HUD가 다시 찾아서 재바인딩하는 문제 방지)
        CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        CurrentWeapon->SetOwner(nullptr);
        CurrentWeapon->SetInstigator(nullptr);

        // 보이기/충돌
        CurrentWeapon->SetActorHiddenInGame(true);
        CurrentWeapon->SetActorEnableCollision(false);
    }

    // 2) 포인터를 완전히 비움 → LMB 눌러도 Fire()에서 바로 return
    CurrentWeapon = nullptr;

    // 3) 무기 타입도 확실히 Hand/None 으로 세팅
    CurrentWeaponType = E_WeaponType::Hand;
}

void ACHCharacter::SwitchToWeapon(E_WeaponType Type)
{
    if (Type == E_WeaponType::None) { SwitchToHand(); return; }

    CurrentWeaponType = Type;
    if (AWeapon* W = GetOrSpawnWeapon(Type))
    {
        EquipWeapon(W);              // HUD 재바인딩 포함
        BP_OnWeaponEquipped(Type);   // ★ BP 이벤트 훅
        OnWeaponEquipped.Broadcast(Type); // ★ 바인더블 델리게이트
    }
}

void ACHCharacter::ToggleAK47()
{
    ToggleWeapon(E_WeaponType::AK47);
}
void ACHCharacter::ToggleM16()
{
    ToggleWeapon(E_WeaponType::M16);
}
void ACHCharacter::TogglePistol()
{
    ToggleWeapon(E_WeaponType::Pistol);
}
void ACHCharacter::ToggleShotgun()
{
    ToggleWeapon(E_WeaponType::Shotgun);
}
void ACHCharacter::ToggleSniperRifle()
{
    ToggleWeapon(E_WeaponType::SniperRifle);
}

void ACHCharacter::ToggleWeapon(E_WeaponType Type)
{
    // 현재 같은 타입이 장착되어 있고 실제 무기가 있으면 -> 맨손으로
    if (CurrentWeaponType == Type && CurrentWeapon != nullptr)
    {
        SwitchToHand();                 // HUD는 내부에서 "- / -"로 갱신
        return;
    }

    // 그 외에는 해당 무기로 스위치 (HUD 자동 재바인딩)
    SwitchToWeapon(Type);
}
