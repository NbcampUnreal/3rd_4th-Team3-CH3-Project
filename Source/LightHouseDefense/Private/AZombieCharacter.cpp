// Fill out your copyright notice in the Description page of Project Settings.


#include "AZombieCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Kismet/GameplayStatics.h"
#include "HealthComponent.h"
#include "HealthSubsystem.h"
#include "AIController.h"

// Sets default values
AAZombieCharacter::AAZombieCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    //캡슐 컴포넌트 기본설정
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

    //캐릭터 이동 컴포넌트 기본설정
    GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 회전
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // 회전 속도
    GetCharacterMovement()->JumpZVelocity = 600.f; // 점프 높이
    GetCharacterMovement()->AirControl = 0.2f; // 공중에서의 이동 제어

    //메시 컴포넌트 생성 및 캡슐 아래에 붙이기
    ZombieMesh = GetMesh();
    if (ZombieMesh)
    {
        ZombieMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f)); // 캡슐 아래로 내리기
        ZombieMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f)); // 메시 회전 (보통 기본 메시가 X축이 앞으로 가기 때문에 Y축으로 회전)
        ZombieMesh->SetCollisionProfileName(TEXT("NoCollision")); // 메시 자체는 충돌하지 않도록 설정
    }
    //AIController 클래스 설정
    AIControllerClass = AAIController::StaticClass();//기본 AIController 클래스를 사용
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // 월드에 배치되거나 스폰될때 AI가 제어

    TeamID = FGenericTeamId(1);

    HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

FGenericTeamId AAZombieCharacter::GetGenericTeamId() const
{
    return TeamID;
}

// Called when the game starts or when spawned
void AAZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (HealthComp)
    {
        HealthComp->Initialize(DefaultMaxHealth, ETeam::Zombie);
    }
	
}

// Called every frame
void AAZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AAZombieCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    UE_LOG(LogTemp, Warning, TEXT("take damage"));
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
  
}

