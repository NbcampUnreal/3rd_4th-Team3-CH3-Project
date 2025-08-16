
#include "AITurretPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include <Kismet/GameplayStatics.h>

// AAITurretPawn 생성자
AAITurretPawn::AAITurretPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    TurretBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBody"));
    SetRootComponent(TurretBody);

    TurretNeckPivot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretNeckPivot"));
    TurretNeckPivot->SetupAttachment(RootComponent);

    TurretNeck = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretNeck"));
    TurretNeck->SetupAttachment(TurretNeckPivot);

    TurretHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretHead"));
    TurretHead->SetupAttachment(TurretNeck);

    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(TurretHead);

    RotationSpeed = 50.0f;
    FireRate = 1.0f;
    CurrentState = ETurretState::Disabled;
}

void AAITurretPawn::BeginPlay()
{
    Super::BeginPlay();

    if (AIControllerClass != nullptr)
    {
        SpawnDefaultController();
    }
    DisableTurret();//테스트용 비활성화 
    //InstallTurret();//테스트용 일단 활성화
}

void AAITurretPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //
    // 비헤이비어 트리가 모든 로직을 제어하므로, Tick 함수에는 추가 로직이 필요하지 않습니다.
    //
}

void AAITurretPawn::SetTurretState(ETurretState NewState)
{
    CurrentState = NewState;
}

void AAITurretPawn::DisableTurret()
{
    CurrentState = ETurretState::Disabled;
    GetWorldTimerManager().ClearTimer(FireTimerHandle);

    AAIController* MyController = Cast<AAIController>(GetController());
    if (MyController)
    {
        MyController->StopMovement();
        // 비헤이비어 트리 실행을 중단하거나, 비활성화 상태를 블랙보드에 설정하여 AI 로직이 멈추도록 유도
    }
    EnableTurretNeckbowdown();
}

void AAITurretPawn::EnableTurret()
{
    CurrentState = ETurretState::Scanning;

    // [추가] 비활성화 상태에서 켤 때 BT가 안 돌고 있다면 여기서 실행
    if (AAIController* MyController = Cast<AAIController>(GetController()))
    {
        // 이미 실행 중이면 알아서 건너뜀
        if (BehaviorTreeAsset)
        {
            // RunBehaviorTree는 여러 번 호출해도 안전(이미 실행 중이면 내부에서 처리)
            MyController->RunBehaviorTree(BehaviorTreeAsset);
        }
    }
}

void AAITurretPawn::Fire()
{
  if(CurrentState==ETurretState::Attacking)
  {

    
    FVector StartLocation = MuzzleLocation->GetComponentLocation();
    FVector ForwardVector = MuzzleLocation->GetForwardVector();
    FVector EndLocation = StartLocation + (ForwardVector * FireRange);

    FHitResult HitResult;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );

    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.1f);
    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor)
        {
            // 데미지주는거 구현해야함
            UGameplayStatics::ApplyDamage(
                HitActor,       // 데미지를 받을 액터
                TurretDamage,           // 적용할 기본 데미지 양
                GetInstigatorController(),// 데미지를 준 컨트롤러 (플레이어 또는 AI) GetInstigatorController()는 액터에게 데미지를준
                this,       // 데미지를 준 액터 (자기자신)
                nullptr // 데미지 유형 
            );
            DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Green, false, 10.0f);
        }
    }
  }
}

void AAITurretPawn::InstallTurret()
{
    EnableTurret();
}

void AAITurretPawn::ScanForTargets(float DeltaTime)
{
    CurrentYawRotation += RotationSpeed * DeltaTime;

    if (CurrentYawRotation >= 360.0f)
    {
        CurrentYawRotation -= 360.0f;
    }

    FRotator NewRotation(0, CurrentYawRotation, 0);
    TurretNeckPivot->SetRelativeRotation(NewRotation);
}

void AAITurretPawn::EnableTurretNeckbowdown()
{
    FRotator NeckBowDown(-50, 0, 50);
    TurretNeckPivot->SetRelativeRotation(NeckBowDown);
}
