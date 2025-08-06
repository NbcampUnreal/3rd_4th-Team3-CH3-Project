#include "LightHouseGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "LightHouseGameInstance.h"
#include "LighthouseHUD.h"
#include "Engine/World.h" // 추가된 헤더

ALightHouseGameModeBase::ALightHouseGameModeBase()
{
    GameStateClass = ALighthouseGameState::StaticClass();
    HUDClass = ALighthouseHUD::StaticClass();
}

void ALightHouseGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // GameInstance를 통해 난이도 시스템 시작 로그 확인
    if (ULightHouseGameInstance* GI = Cast<ULightHouseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode에서 GameInstance 확인 완료"));
    }
}
