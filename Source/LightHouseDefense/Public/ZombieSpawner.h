#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieSpawner.generated.h"

class AAZombieCharacter;
class USceneComponent;

UCLASS()
class LIGHTHOUSEDEFENSE_API AZombieSpawner : public AActor
{
    GENERATED_BODY()

public:
    AZombieSpawner();

    // === 기존 유지 ===
    UFUNCTION(BlueprintCallable, Category = "Spawner") void StopSpawning();
    UFUNCTION(BlueprintCallable, Category = "Spawner") void PauseSpawning();
    UFUNCTION(BlueprintCallable, Category = "Spawner") void ResumeSpawning();

    // FIX: 자동 대신 수동 시작을 위해 추가
    UFUNCTION(BlueprintCallable, Category = "Spawner") void StartSpawning();

protected:
    virtual void BeginPlay() override;

    // === 기존 유지 ===
    void SpawnZombies();       // 내부에서 타이머 세팅
    void SpawnSingleZombie();  // 한 마리 스폰

    // === 기존 필드 유지 ===
    FTimerHandle SpawnTimerHandle;
    int32 SpawnedCount = 0;
    int32 TargetSpawnCount = 0;
    float CurrentSpawnInterval = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Spawner") TSubclassOf<AAZombieCharacter> ZombieClass;
    UPROPERTY(EditAnywhere, Category = "Spawner") TArray<AActor*> SpawnPoints;
};
