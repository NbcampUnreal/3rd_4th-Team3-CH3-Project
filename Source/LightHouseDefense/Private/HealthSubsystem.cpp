#include "HealthSubsystem.h"
#include "HealthComponent.h"
#include "AZombieCharacter.h"
#include "LightHouseGameModeBase.h"
#include "LighthouseGameState.h"
#include "CHCharacter.h"
#include "Engine/World.h"

// game instance subsystem 초기화 함수 (게임 시작 시 한번 실행)
void UHealthSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

// 엑터(플레이 좀비 등)의 HealthComponent를 초기화하는 함수
void UHealthSubsystem::InitializeHealthForActor(AActor* Actor)
{
    if (!Actor) return;

    // 엑터에 HealthComponent가 있으면 초기화
    if (UHealthComponent* HC = Actor->FindComponentByClass<UHealthComponent>())
    {
        // 플레이어일 경우
        if (Actor->IsA(ACHCharacter::StaticClass()))
        {
            //체력 기본값, 팀 설정
            HC->Initialize(DefaultPlayerHP, ETeam::Player);
            // 플레이어는 죽어도 pawn 제거 안함
            HC->bDestroyOwnerOnDeath = false;

            // 사망 이벤트 바인딩 -> handlePlayerDied 호출
            HC->OnDied.AddDynamic(this, &UHealthSubsystem::HandlePlayerDied);
        }
        // 좀비일 경우
        else if (Actor->IsA(AAZombieCharacter::StaticClass()))
        {
            //체력 기본값, 팀 설정
            HC->Initialize(DefaultZombieHP, ETeam::Zombie);
            // 좀비는 죽으면 pawn 제거
            HC->bDestroyOwnerOnDeath = bDestroyZombieOnDeath;

            // 사망 이벤트 바인딩 -> handleZombieDied 호출
            HC->OnDied.AddDynamic(this, &UHealthSubsystem::HandleZombieDied);
        }
        // 그 외 엑터(예: NPC, 오브젝트 등)일 경우
        else
        {
            HC->Initialize(HC->GetMaxHealth(), ETeam::Neutral);
        }
    }
}

// ===== 사망이벤트 처리 =====

//플레이어 사망 시 처리 함수
void UHealthSubsystem::HandlePlayerDied(AActor* Dead)
{
    if (!Dead) return;

    if (UWorld* World = Dead->GetWorld())
    { //game mode 가져오기
        if (ALightHouseGameModeBase* GM = World->GetAuthGameMode<ALightHouseGameModeBase>())
        {
            //game stop run 호출 -> 게임 진행 정지
            GM->StopRun();  // 플레이어 사망 시 러닝 중지 (아래 2번에서 GameMode에 구현)
        }
    }
}
// 좀비 사망 시 처리 함수
void UHealthSubsystem::HandleZombieDied(AActor* Dead)
{
    if (!Dead) return;

    if (UWorld* World = Dead->GetWorld())
    {
        // game state 가져오기
        if (ALighthouseGameState* GS = World->GetGameState<ALighthouseGameState>())
        {
            // 좀비 수를 줄이는 로직 쓰려면 주석 해제 (아직 미완)
            // GS->AddRemainingZombies(-1);
        }
    }
}
