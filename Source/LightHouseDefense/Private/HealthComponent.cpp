#include "HealthComponent.h"
#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    if (AActor* Owner = GetOwner())
    {
        // UE Damage 시스템과 자동 연동(총알/근접 구분 없이 수신)
        Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleAnyDamage);
    }

    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::Initialize(float InMaxHealth, ETeam InTeam)
{
    MaxHealth = FMath::Max(1.f, InMaxHealth);
    Team = InTeam;
    CurrentHealth = MaxHealth;
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::Heal(float Amount)
{
    if (Amount <= 0.f || IsDead()) return;
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::HandleAnyDamage(AActor* DamagedActor, float Damage, const UDamageType*,
    AController* Instigator, AActor* Causer)
{
    if (Damage <= 0.f || IsDead()) return;
    ApplyDamageInternal(Damage, Instigator, Causer);
}

void UHealthComponent::ApplyDamageInternal(float Damage, AController*, AActor*)
{
    const float Old = CurrentHealth;
    CurrentHealth = FMath::Clamp(Old - Damage, 0.f, MaxHealth);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.f)
    {
        OnDied.Broadcast(GetOwner());
        if (bDestroyOwnerOnDeath)
        {
            if (AActor* Owner = GetOwner())
            {
                Owner->Destroy(); // 좀비에 권장
            }
        }
    }
}
