#pragma once

#include "LightHouseDefense/LightHouseDefense.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CHPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class LIGHTHOUSEDEFENSE_API ACHPlayerController : public APlayerController
{
    GENERATED_BODY()


public:
    ACHPlayerController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* InputMappingContext;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* SprintAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* CrouchAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
    UInputAction* RifleAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* ShotgunAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* IA_RifleAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* IA_ShotgunAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
    UInputAction* FireAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* AimAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* ReloadAction;

    virtual void SetupInputComponent() override;

    void OnRifleAction();
    void OnShotgunAction();

protected:
    virtual void BeginPlay() override;

};
