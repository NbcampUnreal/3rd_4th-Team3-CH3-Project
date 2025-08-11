#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HealthSubsystem.generated.h"

class UHealthComponent;
class ACHCharacter;            // ← 실제 플레이어 클래스명 전방선언 (있으면 include로 교체)
class AAZombieCharacter;       // ← 이미 있는 좀비 클래스

UCLASS()
class LIGHTHOUSEDEFENSE_API UHealthSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    void InitializeHealthForActor(AActor* Actor);

    UPROPERTY(EditAnywhere, Category = "Defaults") float DefaultPlayerHP = 100.f;
    UPROPERTY(EditAnywhere, Category = "Defaults") float DefaultZombieHP = 50.f;
    UPROPERTY(EditAnywhere, Category = "Defaults") bool  bDestroyZombieOnDeath = true;

private:
    // ★ Dynamic delegate용 바인딩 대상 함수 (UFUNCTION 필수)
    UFUNCTION() void HandlePlayerDied(AActor* Dead);
    UFUNCTION() void HandleZombieDied(AActor* Dead);
};
