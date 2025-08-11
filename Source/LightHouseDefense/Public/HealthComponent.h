#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8 { Player, Zombie, Neutral };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDied, AActor*, DeadActor);

/** 공용 HP/사망 컴포넌트: Damage 시스템(OnTakeAnyDamage)과 자동 연동 */
UCLASS(ClassGroup = (Systems), meta = (BlueprintSpawnableComponent))
class LIGHTHOUSEDEFENSE_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UHealthComponent();

    UFUNCTION(BlueprintCallable) void Initialize(float InMaxHealth, ETeam InTeam);
    UFUNCTION(BlueprintCallable) float GetHealth() const { return CurrentHealth; }
    UFUNCTION(BlueprintCallable) float GetMaxHealth() const { return MaxHealth; }
    UFUNCTION(BlueprintCallable) bool  IsDead()   const { return CurrentHealth <= 0.f; }
    UFUNCTION(BlueprintCallable) void  Heal(float Amount);

    /** 좀비는 true(사망 즉시 Destroy) 권장, 플레이어는 false 권장 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health") bool  bDestroyOwnerOnDeath = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health") ETeam Team = ETeam::Neutral;

    UPROPERTY(BlueprintAssignable) FOnHealthChanged OnHealthChanged;
    UPROPERTY(BlueprintAssignable) FOnDied         OnDied;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION() void HandleAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
        class AController* Instigator, AActor* DamageCauser);
    void ApplyDamageInternal(float Damage, AController* Instigator, AActor* Causer);

private:
    UPROPERTY(EditAnywhere, Category = "Health", meta = (ClampMin = 1)) float MaxHealth = 100.f;
    UPROPERTY(VisibleInstanceOnly, Category = "Health")              float CurrentHealth = 0.f;
};
