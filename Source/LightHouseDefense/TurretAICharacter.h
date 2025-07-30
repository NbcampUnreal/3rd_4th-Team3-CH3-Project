#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurretAICharacter.generated.h"


UCLASS()
class LIGHTHOUSEDEFENSE_API ATurretAICharacter : public ACharacter
{
	GENERATED_BODY()

public:


public: 
	virtual void Tick(float Deltatime) override;

	ATurretAICharacter();


	//≈Õ∑ø¿« ∏≈Ω√
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret")
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret")
	UStaticMeshComponent* TurretHead;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret")
	UStaticMeshComponent* TurretBody;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret")
	UStaticMeshComponent* Turretneck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret AI")
	bool bIsScanning = true;


};
