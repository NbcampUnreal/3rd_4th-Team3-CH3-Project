#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieSpawner.generated.h"

class AAZombieCharacter;
//class USceneComponent;

UCLASS()
class LIGHTHOUSEDEFENSE_API AZombieSpawner : public AActor
{
    GENERATED_BODY()

public:
    AZombieSpawner();


    // FIX: 자동 대신 수동 시작을 위해 추가
    UFUNCTION(BlueprintCallable, Category = "Spawner") void StartSpawning();
    // === 기존 유지 ===
    UFUNCTION(BlueprintCallable, Category = "Spawner") void StopSpawning();
    UFUNCTION(BlueprintCallable, Category = "Spawner") void PauseSpawning();
    UFUNCTION(BlueprintCallable, Category = "Spawner") void ResumeSpawning();

    

protected:
    virtual void BeginPlay() override;

    // === 기존 유지 ===
    //void SpawnZombies();       // 내부에서 타이머 세팅
    //void SpawnSingleZombie();  // 한 마리 스폰

    // === 기존 필드 유지 ===
    //FTimerHandle SpawnTimerHandle;
    //int32 SpawnedCount = 0;
    //int32 TargetSpawnCount = 0;
    //float CurrentSpawnInterval = 1.0f;

    //UPROPERTY(EditAnywhere, Category = "Spawner") TSubclassOf<AAZombieCharacter> ZombieClass;
    //UPROPERTY(EditAnywhere, Category = "Spawner") TArray<AActor*> SpawnPoints;

private:
    /** 일반 좀비 스폰 클래스 & 인터벌(초) */
    UPROPERTY(EditAnywhere, Category = "Spawner|Normal")
    TSubclassOf<AAZombieCharacter> NormalZombieClass;

    UPROPERTY(EditAnywhere, Category = "Spawner|Normal", meta = (ClampMin = "0.1"))
    float NormalSpawnInterval = 3.0f; // 요구사항: 3초마다 1마리

    /** 탱크 좀비 스폰 클래스 & 인터벌(초) */
    UPROPERTY(EditAnywhere, Category = "Spawner|Tank")
    TSubclassOf<AAZombieCharacter> TankZombieClass;

    UPROPERTY(EditAnywhere, Category = "Spawner|Tank", meta = (ClampMin = "1.0"))
    float TankSpawnInterval = 120.0f; // 요구사항: 2분(120초)마다 1마리

    /** 스폰 위치 후보들 */
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TArray<AActor*> SpawnPoints;

    /** 타이머(일반/탱크) - 무제한 반복 */
    FTimerHandle NormalTimerHandle;
    FTimerHandle TankTimerHandle;

    /** 스폰 1회 수행(일반/탱크) */
    void SpawnSingleNormal();
    void SpawnSingleTank();
    AActor* PickRandomSpawnPoint() const;

};
