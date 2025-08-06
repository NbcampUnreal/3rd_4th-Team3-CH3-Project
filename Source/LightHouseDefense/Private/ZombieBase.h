#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieBase.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API AZombieBase : public ACharacter
{
    GENERATED_BODY()

public:
    AZombieBase();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // 좀비 피격
    UFUNCTION()
    void TakeDamage(float DamageAmount);

    // 죽었는지 확인
    bool IsDead() const;

protected:
    UPROPERTY(EditAnywhere, Category = "Zombie")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Zombie")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Zombie")
    float MovementSpeed = 300.0f;

    // 사망 처리
    void Die();
};
