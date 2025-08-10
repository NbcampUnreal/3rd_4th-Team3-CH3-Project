#include "LightHouseGameInstance.h"
#include "WaveManagerSubsystem.h"

void ULightHouseGameInstance::Init()
{
    Super::Init();

    // 게임 시작 시 웨이브 타이머 시작
    if (UWaveManagerSubsystem* WaveSubsystem = GetSubsystem<UWaveManagerSubsystem>())
    {
        WaveSubsystem->StartWaveProgression();
    }
}
