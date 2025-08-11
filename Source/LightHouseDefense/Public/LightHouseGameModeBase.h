#pragma once

#include "CoreMinimal.h"
#include "LighthouseGameState.h"
#include "GameFramework/GameModeBase.h"
#include "LightHouseGameModeBase.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API ALightHouseGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    ALightHouseGameModeBase();
};
