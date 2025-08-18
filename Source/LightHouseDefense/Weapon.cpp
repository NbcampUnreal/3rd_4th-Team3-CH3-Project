// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "WeaponProjectile.h"
#include "LighthouseHUD.h"                 // [ADD]
#include "GameFramework/PlayerController.h"// [ADD] GetHUD()용
#include "UObject/UnrealType.h"
#include "CHCharacter.h"
#include "E_WeaponType.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"              // 디버그 라인/포인트
#include "Components/TextBlock.h"          // [ADD] UTextBlock 사용

static bool IsBPEnumEqualByName(const UObject* Obj, FName VarName, const TCHAR* WantedName)
{
    if (!Obj) return false;

    // (신) EnumProperty 경로
    if (const FEnumProperty* EP = FindFProperty<FEnumProperty>(Obj->GetClass(), VarName))
    {
        const void* Ptr = EP->ContainerPtrToValuePtr<void>(Obj);
        const int64 Raw = EP->GetUnderlyingProperty()->GetSignedIntPropertyValue(Ptr);
        const UEnum* En = EP->GetEnum();
        if (!En) return false;

        const FString ByName = En->GetNameStringByValue(Raw);
        const FString ByDisp = En->GetDisplayNameTextByValue(Raw).ToString();
        // 둘 중 하나라도 일치하면 true
        return ByName.Equals(WantedName, ESearchCase::IgnoreCase) ||
            ByDisp.Equals(WantedName, ESearchCase::IgnoreCase);
    }

    // (구) ByteProperty + Enum 포인터 경로
    if (const FByteProperty* BP = FindFProperty<FByteProperty>(Obj->GetClass(), VarName))
    {
        const void* Ptr = BP->ContainerPtrToValuePtr<void>(Obj);
        const uint8 Raw = BP->GetPropertyValue(Ptr);
        const UEnum* En = BP->Enum;
        if (!En) return false;

        const FString ByName = En->GetNameStringByValue(Raw);
        const FString ByDisp = En->GetDisplayNameTextByValue(Raw).ToString();
        return ByName.Equals(WantedName, ESearchCase::IgnoreCase) ||
            ByDisp.Equals(WantedName, ESearchCase::IgnoreCase);
    }

    return false;
}

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    // 루트 없으면 생성
    if (!GetRootComponent())
    {
        USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
        SetRootComponent(Root);
    }

    // 총구(Muzzle)
    Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
    if (Muzzle)
    {
        Muzzle->SetupAttachment(GetRootComponent());
    }

    SetActorEnableCollision(false);
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    // 시작 시 값 보정
    CurrentAmmo = FMath::Clamp(CurrentAmmo, 0, MagazineSize);

    // ===== UI 초기 동기화 =====
    OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo); // [ADD]
    OnReloadChanged.Broadcast(bIsReloading);           // [ADD]
    OnCanFireChanged.Broadcast(bCanFire);              // [ADD]
    UpdateAmmoUI();                                    // [ADD] 텍스트 즉시 동기화
}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// [ADD] 위젯에서 텍스트블록 포인터를 넘겨주는 함수
void AWeapon::SetAmmoTextBlock(UTextBlock* InText)
{
    AmmoTextBlock = InText;
    UpdateAmmoUI(); // 연결 즉시 1회 표기
}

void AWeapon::Equip(APawn* NewOwnerPawn, FName SocketName)
{
    if (!NewOwnerPawn) return;

    SetOwner(NewOwnerPawn);
    SetInstigator(NewOwnerPawn);

    if (USkeletalMeshComponent* Mesh = NewOwnerPawn->FindComponentByClass<USkeletalMeshComponent>())
    {
        if (Mesh->DoesSocketExist(SocketName))
        {
            AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
            EquippedSocketName = SocketName;
        }
    }

    SetActorEnableCollision(true);
    SetActorHiddenInGame(false);

    // ===================== [ADD] HUD에 재바인드 요청 =====================
    if (AController* C = NewOwnerPawn->GetController())
    {
        if (APlayerController* PC = Cast<APlayerController>(C))
        {
            if (ALighthouseHUD* HUD = PC->GetHUD<ALighthouseHUD>())
            {
                HUD->RebindAmmoToCurrentWeapon();   // 장착 후 UI 재연결
            }
        }
    }

    if (APlayerController* PC = Cast<APlayerController>(NewOwnerPawn->GetController()))
        if (ALighthouseHUD* HUD = PC->GetHUD<ALighthouseHUD>())
            HUD->RebindAmmoToCurrentWeapon();

    // 이미 TextBlock이 연결돼 있었다면 최신값 한 번 더 밀어줌(안전)
    UpdateAmmoUI();
    // ====================================================================
}

void AWeapon::Reload()
{
    if (bIsReloading)                return;
    if (ReserveAmmo <= 0)            return;
    if (CurrentAmmo >= MagazineSize) return;

    bIsReloading = true;
    OnReloadStarted();
    OnReloadChanged.Broadcast(true); // [ADD]

    GetWorldTimerManager().SetTimer(TH_Reload, this, &AWeapon::FinishReload, ReloadTime, false);
}

void AWeapon::FinishReload()
{
    bIsReloading = false;

    const int32 Need = MagazineSize - CurrentAmmo;
    const int32 ToLoad = FMath::Min(Need, ReserveAmmo);

    CurrentAmmo += ToLoad;
    ReserveAmmo -= ToLoad;

    OnReloadFinished();
    OnReloadChanged.Broadcast(false);                    // [ADD]
    OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);   // [ADD]
    UpdateAmmoUI();                                      // [ADD] UI 갱신
}

void AWeapon::StartFireCooldown()
{
    bCanFire = false;
    OnCanFireChanged.Broadcast(false); // [ADD]

    const float Interval = (FireRate > 0.f) ? (1.f / FireRate) : 0.1f;
    GetWorldTimerManager().SetTimer(
        TH_FireCooldown,
        [this]()
        {
            bCanFire = true;
            OnCanFireChanged.Broadcast(true); // [ADD]
        },
        Interval,
        false
    );
}

void AWeapon::Fire()
{
    // 오너가 플레이어 캐릭터가 아니면(또는 Hand에서 Owner를 null로 만들었다면) 차단
    const ACHCharacter* CH = Cast<ACHCharacter>(GetOwner());
    if (!CH)
        return;

    // Hand 상태면 발사 금지
    if (CH->GetCurrentWeaponType() == E_WeaponType::Hand)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Fire blocked: Hand state"));
        return;
    }

    // 예전 무기 인스턴스에서 호출되는 걸 방지
    if (CH->GetCurrentWeapon() != this)
        return;

    // 발사 가능 여부
    if (!bCanFire || bIsReloading) return;

    // 탄 없음 → 자동 재장전/드라이 파이어
    if (CurrentAmmo <= 0)
    {
        if (bAutoReload && ReserveAmmo > 0) // [ADD]
        {
            Reload(); // [ADD]
        }
        else
        {
            OnDryFire(); // [ADD]
        }
        return;
    }

    // 카메라 위치/방향 (Instigator 우선)
    FVector  CamLoc = FVector::ZeroVector;
    FRotator CamRot = FRotator::ZeroRotator;

    if (AController* C = GetInstigatorController())
    {
        C->GetPlayerViewPoint(CamLoc, CamRot);
    }
    else if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->GetPlayerViewPoint(CamLoc, CamRot);
    }
    else
    {
        // 폴백: 무기 기준
        CamLoc = Muzzle ? Muzzle->GetComponentLocation() : GetActorLocation();
        CamRot = GetActorRotation();
    }

    // 사거리(최소 가드)
    const float TraceDist = FMath::Max(Range, 1000.f); // [MOD]
    const ECollisionChannel TraceChannel = ECC_Visibility;

    // ===== 샷건(다중) / 일반(단일) 분기 =====
    if (PelletCount > 1) // 샷건 모드
    {
        for (int32 i = 0; i < PelletCount; ++i)
        {
            FRotator ShotRot = CamRot;
            if (SpreadAngle > 0.f)
            {
                ShotRot.Pitch += FMath::FRandRange(-SpreadAngle, SpreadAngle);
                ShotRot.Yaw += FMath::FRandRange(-SpreadAngle, SpreadAngle);
            }

            const FVector TraceEnd = CamLoc + ShotRot.Vector() * TraceDist;

            FHitResult Hit;
            FCollisionQueryParams QP(SCENE_QUERY_STAT(ShotgunTrace), true);
            QP.AddIgnoredActor(this);
            if (GetOwner()) QP.AddIgnoredActor(GetOwner());

            if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, TraceChannel, QP))
            {
                UGameplayStatics::ApplyPointDamage(
                    Hit.GetActor(), Damage, ShotRot.Vector(), Hit,
                    GetInstigatorController(), this, nullptr
                );
                DrawDebugLine(GetWorld(), CamLoc, Hit.ImpactPoint, FColor::Red, false, 1.2f, 0, 1.2f);
                DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 6.f, FColor::Yellow, false, 1.2f);
            }
            else
            {
                DrawDebugLine(GetWorld(), CamLoc, TraceEnd, FColor::Blue, false, 1.2f, 0, 1.2f);
            }
        }
    }
    else // 일반 무기: 단일 레이
    {
        const FVector TraceEnd = CamLoc + CamRot.Vector() * TraceDist;

        FHitResult Hit;
        FCollisionQueryParams QP(SCENE_QUERY_STAT(WeaponFireTrace), true);
        QP.AddIgnoredActor(this);
        if (GetOwner()) QP.AddIgnoredActor(GetOwner());

        if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, TraceChannel, QP))
        {
            UGameplayStatics::ApplyPointDamage(
                Hit.GetActor(), Damage, CamRot.Vector(), Hit,
                GetInstigatorController(), this, nullptr
            );
            DrawDebugLine(GetWorld(), CamLoc, Hit.ImpactPoint, FColor::Red, false, 1.2f, 0, 1.2f);
            DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.f, FColor::Yellow, false, 1.2f);
        }
        else
        {
            DrawDebugLine(GetWorld(), CamLoc, TraceEnd, FColor::Blue, false, 1.2f, 0, 1.2f);
        }
    }

    // 후처리
    PlayFireEffect();
    CurrentAmmo = FMath::Max(0, CurrentAmmo - 1);
    OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo); // [ADD]
    UpdateAmmoUI();                                    // [ADD] UI 갱신
    StartFireCooldown();
}

void AWeapon::Unequip()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetOwner(nullptr);
    SetInstigator(nullptr);
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}

// [ADD] 텍스트블록으로 실제 UI 갱신
void AWeapon::UpdateAmmoUI()
{
    if (AmmoTextBlock.IsValid())
    {
        const FText AmmoTxt = FText::Format(
            NSLOCTEXT("Weapon", "AmmoFmt", "{0} / {1}"),
            FText::AsNumber(CurrentAmmo),
            FText::AsNumber(ReserveAmmo)
        );
        AmmoTextBlock->SetText(AmmoTxt);
    }
}
