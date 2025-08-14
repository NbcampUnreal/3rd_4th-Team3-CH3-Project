#include "HealthComponent.h"
#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{ // 매 프레임 실행 방지
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    // 시작 시 현재 체력을 최대 체력으로 초기화

    CurrentHealth = MaxHealth;

    if (AActor* Owner = GetOwner())
    {
        /**
          * UE의 Damage 시스템과 연동
          * - 총알, 근접 공격, 폭발 등 어떤 데미지 타입이든
          * - AActor::TakeDamage() 또는 ApplyDamage()가 호출되면
          * - OnTakeAnyDamage 델리게이트가 실행됨
          * - 여기서 HandleAnyDamage() 함수가 자동 호출됨
          */
        Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleAnyDamage);
    }

    // 체력 변경 브로드캐스트 (UI 갱신 등)
   // OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::Initialize(float InMaxHealth, ETeam InTeam)
{
    //최소 체력 1 이상
    MaxHealth = FMath::Max(1.f, InMaxHealth);
    // team 설정 (Player, Zombie, Neutral 등)
    Team = InTeam;
    // 현재 체력을 최대 체력으로 초기화
    CurrentHealth = MaxHealth;
    // 체력 변경 브로드캐스트 (UI 갱신 등)
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::Heal(float Amount)
{
    // 회복량이 0 이하이거나 이미 사망한 경우 무시
    if (Amount <= 0.f || IsDead()) return;
    // 현재 체력을 최대 체력으로 제한
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
    // 체력 변경 브로드캐스트 (UI 갱신 등)
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::HandleAnyDamage(
    AActor* DamagedActor, // 데미지를 받은 액터(get Owner()와 동일)
    float Damage, // 받은 데미지 양
    const UDamageType*, // 데미지 타입 (사용하지 않음)
    AController* Instigator, // 데미지를 준 컨트롤러
    AActor* Causer) // 데미지를 준 액터 (총알, 폭발 등)
{
    // 데미지 양이 0 이하이거나 이미 사망한 경우 무시
    if (Damage <= 0.f || IsDead()) return;
    // 데미지 적용 함수 호출
    ApplyDamageInternal(Damage, Instigator, Causer);
}

void UHealthComponent::ApplyDamageInternal(float Damage, AController*, AActor*)
{
    // 기존 체력 저장
    const float Old = CurrentHealth;
    // 데미지 반영 (0 이하로는 제한)
    CurrentHealth = FMath::Clamp(Old - Damage, 0.f, MaxHealth);
    // 체력 변경 브로드캐스트 (UI 갱신 등)
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

    UE_LOG(LogTemp, Warning, TEXT("Current HP: %f / Max HP: %f"), CurrentHealth, MaxHealth);
    //체력이 0 이하로 떨어진 경우 사망 처리
    if (CurrentHealth <= 0.f)
    {//사망 알림 브로드캐스트 (UhealthSubsystem 등에서 처리)
        OnDied.Broadcast(GetOwner());
        // bDestroyOwnerOnDeath가 true인 경우, 소유엑터 삭제
        if (bDestroyOwnerOnDeath)
        {
            if (AActor* Owner = GetOwner())
            {
                Owner->Destroy(); // 좀비에 권장 (플레이어는 보통 false)
            }
        }
    }
}
