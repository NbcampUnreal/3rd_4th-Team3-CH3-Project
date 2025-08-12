#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HealthSubsystem.generated.h"


class UHealthComponent;
class ACHCharacter;           
class AAZombieCharacter;      


//healthSubsystem
// gameinstance 단위로 존재하는 hp관리 서브시스템
// 게임 내 액터 (플레이어, 좀비 등)의 체력 초기화 및 사망 처리 로직 연결
// ondied 델리게이트를 통해 사망 시 gamemode / gamestate에 알림
UCLASS()
class LIGHTHOUSEDEFENSE_API UHealthSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // subsystem 초기화 시 호출 (게임 시작 시 한번 실행)
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    //엑터에 HealthComponent를 초기화
    // 팀 타입 (플레이어, 좀비 등)에 따라 기본 체력 설정
    //사망 시 처리할 델리게이트 바인딩
    void InitializeHealthForActor(AActor* Actor);

    // 플레이어시 기본 체력
    UPROPERTY(EditAnywhere, Category = "Defaults")
    float DefaultPlayerHP = 100.f;

    // 좀비 기본 체력
    UPROPERTY(EditAnywhere, Category = "Defaults")
    float DefaultZombieHP = 50.f;

    // 좀비 사망 시 엑터 자동 제거 여부
    // true: 좀비 사망 시 Pawn 제거 (게임에서 사라짐)
    // false: 다른 처리여부 (리스폰 등)
    UPROPERTY(EditAnywhere, Category = "Defaults")
    bool bDestroyZombieOnDeath = true;

private:
    // 플레이어 사망 처리 함수 (ondied 델리게이트 연결용)
    UFUNCTION()
    void HandlePlayerDied(AActor* Dead);

    // 좀비 사망 처리 함수 (ondied 델리게이트 연결용)
    UFUNCTION()
    void HandleZombieDied(AActor* Dead);
};
