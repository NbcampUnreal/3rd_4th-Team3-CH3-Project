#include "ZombieBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AZombieBase::AZombieBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // 이동 속도 설정
    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AZombieBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
}

void AZombieBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 간단한 AI 이동 등은 여기에 구현 가능
}

void AZombieBase::TakeDamage(float DamageAmount)
{
    CurrentHealth -= DamageAmount;

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void AZombieBase::Die()
{
    // 죽었을 때 처리 (사운드, 파티클, 제거 등)
    Destroy(); // 임시로 제거
}

bool AZombieBase::IsDead() const
{
    return CurrentHealth <= 0.0f;
}
