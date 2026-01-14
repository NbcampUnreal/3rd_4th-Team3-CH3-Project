#include "UWeaponUnlockSubsystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "HealthComponent.h"
#include "AZombieCharacter.h"
#include "TankZombieCharacter.h"
#include "LighthouseHUD.h"
#include "GameFramework/PlayerController.h"

FString UWeaponUnlockSubsystem::GetWeaponDisplayName(E_WeaponType Type) const
{
    if (const UEnum* Enum = StaticEnum<E_WeaponType>())
        return Enum->GetDisplayNameTextByValue((int64)Type).ToString();
    return TEXT("Weapon");
}

int32 UWeaponUnlockSubsystem::GetHudSlotForWeapon(E_WeaponType Type) const
{
    switch (Type)
    {
    case E_WeaponType::AK47:        return 1;
    case E_WeaponType::M16:         return 2;
    case E_WeaponType::Shotgun:     return 3;
    case E_WeaponType::SniperRifle: return 4;
    default:                        return 1;
    }
}

void UWeaponUnlockSubsystem::ForceUnlock(E_WeaponType Type, bool bNotifyHUD /*=true*/)
{
    if (Type == E_WeaponType::None || Type == E_WeaponType::Hand) return;
    if (Unlocked.Contains(Type)) return;

    Unlocked.Add(Type);

    if (bNotifyHUD)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            if (ALighthouseHUD* HUD = PC->GetHUD<ALighthouseHUD>())
            {
                HUD->ShowUnlockText(GetHudSlotForWeapon(Type),
                    FString::Printf(TEXT("%s 해금!"), *GetWeaponDisplayName(Type)));
            }
        }
    }

    OnWeaponUnlocked.Broadcast(Type, TotalKills);
}

void UWeaponUnlockSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 요구치 설정 (원하는 값으로 수정 가능)
    RequiredKills.Add(E_WeaponType::Pistol, 0);
    RequiredKills.Add(E_WeaponType::AK47, 10);
    RequiredKills.Add(E_WeaponType::M16, 30);
    RequiredKills.Add(E_WeaponType::Shotgun, 60);
    RequiredKills.Add(E_WeaponType::SniperRifle, 80);

    // 시작 시 즉시 해금되는(요구치 0) 무기 처리
    for (const auto& KV : RequiredKills)
    {
        if (KV.Value <= 0) Unlocked.Add(KV.Key);
    }

    // 월드 생성/정리 시 재바인딩
    PostWorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(
        this, &UWeaponUnlockSubsystem::OnPostWorldInit);
    WorldCleanupHandle = FWorldDelegates::OnWorldCleanup.AddUObject(
        this, &UWeaponUnlockSubsystem::OnWorldCleanup);

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
    // 엔진 델리게이트 해제
    if (PostWorldInitHandle.IsValid())
        FWorldDelegates::OnPostWorldInitialization.Remove(PostWorldInitHandle);
    if (WorldCleanupHandle.IsValid())
        FWorldDelegates::OnWorldCleanup.Remove(WorldCleanupHandle);

    // 월드별 스폰 훅 해제
    for (auto& Pair : SpawnHandles)
        if (UWorld* W = Pair.Key.Get())
            W->RemoveOnActorSpawnedHandler(Pair.Value);
    SpawnHandles.Empty();

    Super::Deinitialize();
}

void UWeaponUnlockSubsystem::OnPostWorldInit(UWorld* World, const UWorld::InitializationValues)
{
    if (!World || !World->IsGameWorld()) return; // PIE 프리뷰/에디터 제외

    RegisterSpawnHook(World);
    // 이미 떠 있는 좀비도 묶어줌
    for (TActorIterator<AActor> It(World); It; ++It) TryBindZombie(*It);

    UE_LOG(LogTemp, Log, TEXT("[Unlock] Registered in world: %s"), *World->GetName());
}

void UWeaponUnlockSubsystem::OnWorldCleanup(UWorld* World, bool, bool)
{
    UnregisterSpawnHook(World);

    // stale 좀비 제거
    for (auto It = BoundZombies.CreateIterator(); It; ++It)
        if (!It->IsValid() || (It->Get()->GetWorld() == World)) It.RemoveCurrent();
}

void UWeaponUnlockSubsystem::RegisterSpawnHook(UWorld* World)
{
    if (SpawnHandles.Contains(World)) return;
    FDelegateHandle H = World->AddOnActorSpawnedHandler(
        FOnActorSpawned::FDelegate::CreateUObject(this, &UWeaponUnlockSubsystem::OnActorSpawned));
    SpawnHandles.Add(World, H);
}

void UWeaponUnlockSubsystem::UnregisterSpawnHook(UWorld* World)
{
    if (FDelegateHandle* H = SpawnHandles.Find(World))
    {
        if (World) World->RemoveOnActorSpawnedHandler(*H);
        SpawnHandles.Remove(World);
    }
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

            // 무기 해금 토스트
            if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
            {
                if (ALighthouseHUD* HUD = PC->GetHUD<ALighthouseHUD>())
                {
                    const FString Msg = FString::Printf(
                        TEXT("%s 해금! (총 %d킬)"),
                        *GetWeaponDisplayName(Type),
                        TotalKills
                    );
                    HUD->ShowWeaponUnlockText(Msg);
                }
            }

            OnWeaponUnlocked.Broadcast(Type, TotalKills);
        }
    }
}
