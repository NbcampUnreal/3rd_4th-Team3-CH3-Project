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

    UFUNCTION(BlueprintCallable)
    void StopRun();

    UFUNCTION(BlueprintCallable)
    void StartRun();

private:
    // StopRun 중복 실행 방지 (PIE/새 게임 시작마다 리셋됨)
    UPROPERTY(VisibleInstanceOnly, Category = "Run State")
    bool bRunStopped = false;
};

