#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieSpawner.generated.h"

class AAZombieCharacter; // 좀비 베이스 클래스 (미리 선언)
class USceneComponent;

UCLASS()
class LIGHTHOUSEDEFENSE_API AZombieSpawner : public AActor
{
    GENERATED_BODY()

public:
    AZombieSpawner();

    UFUNCTION(BlueprintCallable, Category = "Spawner") void StopSpawning();
    UFUNCTION(BlueprintCallable, Category = "Spawner") void PauseSpawning();
    UFUNCTION(BlueprintCallable, Category = "Spawner") void ResumeSpawning();

protected:
    virtual void BeginPlay() override;

    // 좀비 스폰 함수
    void SpawnZombies();

    // 좀비 하나 스폰
    void SpawnSingleZombie();

    // 반복 스폰용 타이머
    FTimerHandle SpawnTimerHandle;

    // 현재 몇 마리 스폰했는지 추적
    int32 SpawnedCount = 0;

    // 타이머가 다 끝났는지 체크
    int32 TargetSpawnCount = 0;

    float CurrentSpawnInterval = 1.0f;

    /** 좀비 블루프린트 클래스 설정 */
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TSubclassOf<AAZombieCharacter> ZombieClass;

    /** 좀비 스폰 위치 배열 */
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TArray<AActor*> SpawnPoints;
};
