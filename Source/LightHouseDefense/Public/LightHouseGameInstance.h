#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LightHouseGameInstance.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API ULightHouseGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
};
