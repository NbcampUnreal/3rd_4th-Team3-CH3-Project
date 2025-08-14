#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HealthSubsystem.generated.h"

class UHealthComponent;
class ACHCharacter;                 // 플레이어 Pawn
class AAZombieCharacter;            // 일반 좀비 베이스
class ALightHouseCharacter;
class ATankZombieCharacter;         // 탱크 좀비(있을 때만 사용)

/**
 * UHealthSubsystem
 * - 게임 내 액터의 HealthComponent 초기화/사망 처리 연결(중앙 허브)
 * - "좀비"는 스폰 시 Alive++ 등록, 사망 시 Alive-- & Kill(일반/탱크)++
 * - HUD 갱신은 LighthouseGameState의 델리게이트를 통해 즉시 반영
 *
 * 사용 방법:
 *   스폰 직후 HS->InitializeHealthForActor(Actor) 한 번만 호출하면
 *   Alive 카운트 & 사망 처리 & UI 반영이 전부 자동으로 동작합니다.
 */
UCLASS()
class LIGHTHOUSEDEFENSE_API UHealthSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    /** 게임 시작 시 1회 초기화 (필요 시 로깅/설정 가능) */
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /**
     * 스폰/시작 직후 액터의 HealthComponent 초기화 + 중앙 등록
     * - 플레이어  : 팀 Player, 죽어도 Destroy 안 함
     * - 좀비(일반/탱크): 팀 Zombie, 죽으면 Destroy 함(옵션)
     * - 그 외     : 팀 Neutral
     */
    void InitializeHealthForActor(AActor* Actor);

    /** 기본값(에디터 조정 가능) */
    UPROPERTY(EditAnywhere, Category = "Defaults") float DefaultPlayerHP = 100.f;
    UPROPERTY(EditAnywhere, Category = "Defaults") float DefaultZombieHP = 50.f;

    /** 좀비 사망 시 즉시 Destroy 여부 (플레이어는 항상 false 권장) */
    UPROPERTY(EditAnywhere, Category = "Defaults") bool  bDestroyZombieOnDeath = true;

private:
    /** 플레이어 사망 → GameMode.StopRun() 호출 */
    UFUNCTION() void HandlePlayerDied(AActor* Dead);

    /** 좀비 사망 → Kill++(일반/탱크) & Alive--, 중복 방지 */
    UFUNCTION() void HandleZombieDied(AActor* Dead);

    /** 이 액터가 "좀비(일반/탱크 포함)"인지 판별 (파생 클래스 or Tag="Zombie") */
    bool IsZombieActor(AActor* Actor) const;

    /** 이 액터가 "탱크 좀비"인지 판별 (ATankZombieCharacter 파생) */
    bool IsTankZombie(AActor* Actor) const;

    /**
     * 좀비 등록:
     * - Alive 총합 +1 (GameState에 브로드캐스트 → HUD 즉시 반영)
     * - OnDied 구독(중복 방지)
     * - 내부 TrackedZombies 에 기록(중복 등록/감소 방지)
     */
    void RegisterZombie(AActor* Actor, UHealthComponent* HC);

private:
    /** 등록된 좀비 집합 (Destroy/GC 안전 위해 WeakObjectPtr 활용) */
    UPROPERTY() TSet<TWeakObjectPtr<AActor>> TrackedZombies;
};
