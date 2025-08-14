// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "WeaponProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h" //  발사체 속도 접근용
#include "Components/SceneComponent.h"  //  Muzzle(총구) 컴포넌트
#include "GameFramework/Pawn.h" //  Instigator(발사 주체) 설정
#include "Components/SkeletalMeshComponent.h" //  무기를 소켓에 부착
#include "Engine/World.h"
#include "TimerManager.h"

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    // ★ 루트 컴포넌트가 없으면 생성
    if (!GetRootComponent())
    {
        USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
        SetRootComponent(Root);
    }

    //  총구(Muzzle) 생성 및 루트에 부착
    Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
    if (Muzzle)
    {
        Muzzle->SetupAttachment(GetRootComponent());
    }

    //  기본적으로 충돌 비활성화
    SetActorEnableCollision(false);
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    //  현재 탄약을 탄창 크기 범위 내로 보정
    CurrentAmmo = FMath::Clamp(CurrentAmmo, 0, MagazineSize);
}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeapon::Equip(APawn* NewOwnerPawn, FName SocketName)
{
    if (!NewOwnerPawn) return;

    SetOwner(NewOwnerPawn);       //  소유자 설정
    SetInstigator(NewOwnerPawn);  //  발사 주체 설정

    // ★ 캐릭터 SkeletalMesh에서 지정 소켓에 무기 부착
    if (USkeletalMeshComponent* Mesh = NewOwnerPawn->FindComponentByClass<USkeletalMeshComponent>())
    {
        if (Mesh->DoesSocketExist(SocketName))
        {
            AttachToComponent(
                Mesh,
                FAttachmentTransformRules::SnapToTargetIncludingScale,
                SocketName
            );

            EquippedSocketName = SocketName;
        }
    }

    SetActorEnableCollision(true);  // 충돌 활성화
    SetActorHiddenInGame(false);    // 표시
}

void AWeapon::Reload() //  재장전 시작
{
    if (bIsReloading)              return; // 이미 재장전 중이면 중단
    if (ReserveAmmo <= 0)          return; // 예비 탄약 없음
    if (CurrentAmmo >= MagazineSize) return; // 탄창 가득 참

    bIsReloading = true;
    OnReloadStarted(); //  BP에서 재장전 애니/사운드 재생

    //  일정 시간 후 재장전 완료
    GetWorldTimerManager().SetTimer(TH_Reload, this, &AWeapon::FinishReload, ReloadTime, false);
}

void AWeapon::FinishReload() //  재장전 완료
{
    bIsReloading = false;

    // 필요 탄 수 계산
    const int32 Need = MagazineSize - CurrentAmmo;
    const int32 ToLoad = FMath::Min(Need, ReserveAmmo);

    // 탄창 채우기
    CurrentAmmo += ToLoad;
    ReserveAmmo -= ToLoad;

    OnReloadFinished(); //  BP에서 재장전 완료 애니/사운드
}

void AWeapon::StartFireCooldown() //  발사 후 쿨다운
{
    bCanFire = false;
    const float Interval = (FireRate > 0.f) ? (1.f / FireRate) : 0.1f;
    GetWorldTimerManager().SetTimer(TH_FireCooldown, [this]()
        {
            bCanFire = true;
        }, Interval, false);
}

void AWeapon::Fire()
{
    // ★ 발사 가능 여부 체크
    if (!bCanFire || bIsReloading) return;
    if (CurrentAmmo <= 0)
    {
        if (ReserveAmmo > 0) Reload(); //  자동 재장전
        return;
    }

    // ★ 카메라 위치/방향 가져오기
    FVector CamLoc;
    FRotator CamRot;
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->GetPlayerViewPoint(CamLoc, CamRot);
    }

    // ★ 라인트레이스 거리와 끝점 계산
    const float TraceDist = 20000.f;
    const FVector TraceEnd = CamLoc + CamRot.Vector() * TraceDist;

    // ★ 라인트레이스 설정 (자기 자신/소유자 제외)
    FHitResult Hit;
    FCollisionQueryParams QP(SCENE_QUERY_STAT(WeaponFireTrace), true);
    QP.AddIgnoredActor(this);
    if (GetOwner()) QP.AddIgnoredActor(GetOwner());

    // ★ 라인트레이스 실행
    if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, QP))
    {
        // 맞은 액터에 데미지 적용
        UGameplayStatics::ApplyPointDamage(
            Hit.GetActor(),
            Damage,
            CamRot.Vector(),
            Hit,
            GetInstigatorController(),
            this,
            nullptr
        );

        // 디버그 라인 (빨간색 = 명중)
        DrawDebugLine(GetWorld(), CamLoc, Hit.ImpactPoint, FColor::Red, false, 1.0f, 0, 1.0f);
    }
    else
    {
        // 디버그 라인 (파란색 = 빗나감)
        DrawDebugLine(GetWorld(), CamLoc, TraceEnd, FColor::Blue, false, 1.0f, 0, 1.0f);
    }

    PlayFireEffect(); // ★ BP에서 총구 이펙트 재생
    CurrentAmmo = FMath::Max(0, CurrentAmmo - 1); // 탄 소모
    StartFireCooldown(); // 발사 쿨다운 시작
}

void AWeapon::Unequip()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetOwner(nullptr);
    SetInstigator(nullptr);
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}



