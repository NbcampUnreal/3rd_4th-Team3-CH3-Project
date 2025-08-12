#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


// 팀 종류: 플레이어, 좀비, 오브젝트(건물등) 
UENUM(BlueprintType)
enum class ETeam : uint8 { Player, Zombie, Neutral };


// 체력 변경 이벤트 델리게이트:
// NewHP 변경된 현재 체력
// MaxHP 최대 체력
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHP, float, MaxHP);

// 사망시 호출되는 델리게이트:
// DeadActor 사망한 Actor
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDied, AActor*, DeadActor);

// HealthComponent
// - 액터의 체력, 데미지, 사망 처리담당
// - amage 시스템과 연동
// - 총알 , 근접 공격, 폭발 등 다양한 공격 소스를 구분 없이 처리
UCLASS(ClassGroup = (Systems), meta = (BlueprintSpawnableComponent))
class LIGHTHOUSEDEFENSE_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    
    UHealthComponent();

    //체력과 팀 초기화
    // - InMaxHealth: 최대 체력
    // - InTeam: 소속 팀 (Player, Zombie, Neutral 등)
    UFUNCTION(BlueprintCallable)
    void Initialize(float InMaxHealth, ETeam InTeam);

    // 현재 체력 반환
    UFUNCTION(BlueprintCallable)
    float GetHealth() const { return CurrentHealth; }

    //최대체력 반환
    UFUNCTION(BlueprintCallable)
    float GetMaxHealth() const { return MaxHealth; }

    // 사망여부 확인(체력이 0 이하인지)
    UFUNCTION(BlueprintCallable)
    bool IsDead() const { return CurrentHealth <= 0.f; }

    // 체력 회복
    UFUNCTION(BlueprintCallable)
    void  Heal(float Amount);

    // 시망시 소유 엑터 제거 여부
    // 좀비 : true (죽으면 제거)
    // 플레이어 : false (죽어도 pawn 제거 안함 , UI 리셋처리)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    bool  bDestroyOwnerOnDeath = false;

    // 소속 팀
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    ETeam Team = ETeam::Neutral;

    // 체력 변경시 브로드캐스트 되는 델리게이트 (UI 갱신 등)
    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;

    // 사망시 브로드캐스트 되는 델리게이트( subsystem , GameMode , gamestate 등에서 처리)
    UPROPERTY(BlueprintAssignable)
    FOnDied OnDied;

protected:
    // 컴포넌트가 생성될 때 호출 (UE Damage 시스템과 연동)
    virtual void BeginPlay() override;

private:

    // UE Damage 시스템에서 호출되는 함수
    UFUNCTION()
    void HandleAnyDamage(AActor* DamagedActor,float Damage, const class UDamageType* DamageType,
        class AController* Instigator, AActor* DamageCauser);

    // 내부 데미지 적용 처리 (체력감소, 사망판정, 파괴여부  )
    void ApplyDamageInternal(float Damage, AController* Instigator, AActor* Causer);

private:

    // 최대체력 (에디터에서 조정 가능, 최소 1이상)
    UPROPERTY(EditAnywhere, Category = "Health", meta = (ClampMin = 1))
    float MaxHealth = 100.f;

    // 현재 체력 (런타임에만 표시)
    UPROPERTY(VisibleInstanceOnly, Category = "Health")
    float CurrentHealth = 0.f;
};
