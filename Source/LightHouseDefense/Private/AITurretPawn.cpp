#include "AITurretPawn.h"
#include "Components/StaticMeshComponent.h" 
#include "Components/SceneComponent.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"

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
    CurrentState = ETurretState:: Disabled;


}

void AAITurretPawn::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay에서 즉시 AIController를 포제스하도록 설정
    if (AIControllerClass != nullptr)
    {
        SpawnDefaultController();
    }
    //InstallTurret();//일단 태스트용으로 시작하면 바로 scanning으로 전환
    
}


void AAITurretPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (CurrentState == ETurretState::Scanning)
    {
        ScanForTargets(DeltaTime);
    }
    if (CurrentState == ETurretState::Disabled)
    {
        DisableTurret();
    }

}

void AAITurretPawn::DisableTurret()
{
    CurrentState = ETurretState::Disabled;
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
    // 필요한 경우 AI Controller의 행동을 중지
    AAIController* MyController = Cast<AAIController>(GetController());
    if (MyController)
    {
        MyController->StopMovement();
        // 비헤이비어 트리 실행을 중단하거나, Disable 상태를 블랙보드에 설정하여 AI 로직이 멈추도록 유도
    }
    EnableTurretNeckbowdown();
}

void AAITurretPawn::EnableTurret()
{
    CurrentState = ETurretState::Scanning;
    
}

void AAITurretPawn::Fire()
{
    FVector StartLocation = MuzzleLocation ->GetComponentLocation();
    FVector ForwardVector = MuzzleLocation ->GetForwardVector();
    FVector EndLocation = StartLocation + (ForwardVector * FireRange);

    FHitResult HitResult;


    //히트스캔용 쿼리 파라미터
    FCollisionQueryParams  QueryParams;
    QueryParams.AddIgnoredActor(this);// 자기무시

    //월드에서 라인트래이스 수행
    //ECC_Visibility는 모든물체와 충돌하게 하는 채널 이라고함
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );

    //디버그용

    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.1f);
    if(bHit)
    {
        //맞은 액터정보 가져오기 
        AActor* HitActor = HitResult.GetActor();
        if (HitActor)
        {
            //데미지주는거 구현해야함
            //디버그용
            DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Green, false, 2.0f);

        }
    }
}

void AAITurretPawn::InstallTurret()
{
    EnableTurret(); // 설치 시 터렛 활성화
}

void AAITurretPawn::ScanForTargets(float DeltaTime)
{
    // DeltaTime을 사용하여 매 프레임 일정한 속도로 회전량을 더합니다.
    CurrentYawRotation += RotationSpeed * DeltaTime;

    // 회전 값이 360도를 넘지 않도록 합니다.
    if (CurrentYawRotation >= 360.0f)
    {
        CurrentYawRotation -= 360.0f;
    }

    FRotator NewRotation(0, CurrentYawRotation, 0);

    // TurretNeckPivot의 상대 회전값을 설정합니다.
    // TurretNeck과 TurretHead가 TurretNeckPivot에 부착되어 있으므로 함께 회전합니다.
    TurretNeckPivot->SetRelativeRotation(NewRotation);
}
void AAITurretPawn::EnableTurretNeckbowdown()
{
    FRotator NeckBowDown(50, 0, 50);

    TurretNeckPivot->SetRelativeRotation(NeckBowDown);
}
