#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "E_WeaponType.h"
#include "UWeaponUnlockSubsystem.generated.h"

class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponUnlocked, E_WeaponType, WeaponType, int32, TotalKills);

UCLASS()
class LIGHTHOUSEDEFENSE_API UWeaponUnlockSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // 킬 증가(좀비 사망 시 호출)
    UFUNCTION(BlueprintCallable, Category = "Unlock")
    void RegisterKill();

    // 무기 해금 여부 질의 (무기 Fire에서 사용)
    UFUNCTION(BlueprintCallable, Category = "Unlock")
    bool IsUnlocked(E_WeaponType Type) const;

    // HUD 등에서 바인딩할 수 있는 이벤트
    UPROPERTY(BlueprintAssignable) FOnWeaponUnlocked OnWeaponUnlocked;

    // 총 킬 수 반환(디버그/표시용)
    UFUNCTION(BlueprintPure, Category = "Unlock")
    int32 GetTotalKills() const { return TotalKills; }

    // ▼ 추가: 터렛 해금 시 강제 해금용
    UFUNCTION(BlueprintCallable, Category = "Unlock")
    void ForceUnlock(E_WeaponType Type, bool bNotifyHUD = true);

protected:
    void TryBindExistingZombies();       // 시작 시 월드에 있는 좀비들 바인딩
    void OnActorSpawned(AActor* Spawned);
    void TryBindZombie(AActor* Actor);
    UFUNCTION() void HandleZombieDied(AActor* DeadActor);

    void CheckUnlocks();                 // 현재 킬 수로 해금 가능한 무기 해금

private:
    FDelegateHandle SpawnHandle;
    UPROPERTY() TSet<TWeakObjectPtr<AActor>> BoundZombies;

    // 누적 킬
    UPROPERTY(VisibleAnywhere) int32 TotalKills = 0;

    // 무기별 요구 킬 수
    UPROPERTY(EditAnywhere, Category = "Unlock")
    TMap<E_WeaponType, int32> RequiredKills; // {Pistol:0, AK47:20, M16:40, Shotgun:60, Sniper:80}

    // 해금된 무기
    UPROPERTY(VisibleAnywhere) TSet<E_WeaponType> Unlocked;

    // 한 번만 초기화하려는 플래그
    bool bInitialized = false;

    FString GetWeaponDisplayName(E_WeaponType Type) const;
    int32   GetHudSlotForWeapon(E_WeaponType Type) const;

    // 엔진 월드 라이프사이클 훅
    void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IV);
    void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

    // 월드별 스폰 핸들(레벨 전환 대비)
    TMap<TWeakObjectPtr<UWorld>, FDelegateHandle> SpawnHandles;

    FDelegateHandle PostWorldInitHandle;
    FDelegateHandle WorldCleanupHandle;

    void RegisterSpawnHook(UWorld* World);
    void UnregisterSpawnHook(UWorld* World);
};
