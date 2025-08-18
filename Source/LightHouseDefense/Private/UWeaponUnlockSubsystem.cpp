#include "UWeaponUnlockSubsystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "HealthComponent.h"
#include "AZombieCharacter.h"
#include "TankZombieCharacter.h"
#include "LighthouseHUD.h"
#include "GameFramework/PlayerController.h"

void UWeaponUnlockSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 요구치 설정 (원하는 값으로 수정 가능)
    RequiredKills.Add(E_WeaponType::Pistol, 0);
    RequiredKills.Add(E_WeaponType::AK47, 20);
    RequiredKills.Add(E_WeaponType::M16, 40);
    RequiredKills.Add(E_WeaponType::Shotgun, 60);
    RequiredKills.Add(E_WeaponType::SniperRifle, 80);

    // 시작 시 즉시 해금되는(요구치 0) 무기 처리
    for (const auto& KV : RequiredKills)
    {
        if (KV.Value <= 0) Unlocked.Add(KV.Key);
    }

    // 월드 액터 스폰 훅: 앞으로 스폰되는 좀비도 자동 바인딩
    if (UWorld* W = GetWorld())
    {
        SpawnHandle = W->AddOnActorSpawnedHandler(
            FOnActorSpawned::FDelegate::CreateUObject(this, &UWeaponUnlockSubsystem::OnActorSpawned)
        );
    }

    // 이미 존재하는 좀비들 바인딩
    TryBindExistingZombies();

    bInitialized = true;
}

void UWeaponUnlockSubsystem::Deinitialize()
{
    if (UWorld* W = GetWorld())
    {
        if (SpawnHandle.IsValid())
            W->RemoveOnActorSpawnedHandler(SpawnHandle);
    }
    Super::Deinitialize();
}

void UWeaponUnlockSubsystem::TryBindExistingZombies()
{
    if (!GetWorld()) return;
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        TryBindZombie(*It);
    }
}

void UWeaponUnlockSubsystem::OnActorSpawned(AActor* Spawned)
{
    TryBindZombie(Spawned);
}

// HealthComponent.Team == Zombie 인 액터만 바인딩 (TurretUnlockManager와 같은 패턴)
void UWeaponUnlockSubsystem::TryBindZombie(AActor* Actor)
{
    if (!Actor) return;

    if (UHealthComponent* HC = Actor->FindComponentByClass<UHealthComponent>())
    {
        if (HC->Team == ETeam::Zombie)
        {
            if (!BoundZombies.Contains(Actor))
            {
                HC->OnDied.AddDynamic(this, &UWeaponUnlockSubsystem::HandleZombieDied);
                BoundZombies.Add(Actor);
            }
        }
    }
}

void UWeaponUnlockSubsystem::HandleZombieDied(AActor* DeadActor)
{
    // 필요 시 좀비/탱크 구분을 해서 ‘총 킬’ 외에 세부 통계를 나눌 수도 있음.
    RegisterKill();
}

void UWeaponUnlockSubsystem::RegisterKill()
{
    ++TotalKills;
    CheckUnlocks();
}

bool UWeaponUnlockSubsystem::IsUnlocked(E_WeaponType Type) const
{
    return Unlocked.Contains(Type);
}

void UWeaponUnlockSubsystem::CheckUnlocks()
{
    // 새로 열릴 수 있는 무기들 전부 확인
    for (const auto& KV : RequiredKills)
    {
        const E_WeaponType Type = KV.Key;
        const int32        Need = KV.Value;

        if (!Unlocked.Contains(Type) && TotalKills >= Need)
        {
            Unlocked.Add(Type);

            // HUD 알려주기
            if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
            {
                if (ALighthouseHUD* HUD = PC->GetHUD<ALighthouseHUD>())
                {
                    FString Name;
                    switch (Type)
                    {
                    case E_WeaponType::Pistol:       Name = TEXT("Pistol"); break;
                    case E_WeaponType::AK47:         Name = TEXT("AK47"); break;
                    case E_WeaponType::M16:          Name = TEXT("M16"); break;
                    case E_WeaponType::Shotgun:      Name = TEXT("Shotgun"); break;
                    case E_WeaponType::SniperRifle:  Name = TEXT("Sniper Rifle"); break;
                    default:                         Name = TEXT("Weapon"); break;
                    }
                    const FString Msg = FString::Printf(TEXT("%s 해금! (총 %d킬)"), *Name, TotalKills);
                    HUD->ShowUnlockText(static_cast<int32>(Type), Msg); // HUD에 이미 비슷한 토스트가 있음
                }
            }

            OnWeaponUnlocked.Broadcast(Type, TotalKills);
        }
    }
}
