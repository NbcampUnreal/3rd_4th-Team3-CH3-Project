#pragma once  

#include "CoreMinimal.h"  
#include "GameFramework/Character.h"  
#include "TurretAICharacter.generated.h"  

UCLASS()  
class LIGHTHOUSEDEFENSE_API ATurretAICharacter : public ACharacter  
{  
	GENERATED_BODY()  

public:  
	virtual void Tick(float Deltatime) override;  

	ATurretAICharacter();  

	// 터렛의 매시  
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret")  
	USceneComponent* Root;  
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret")  
	UStaticMeshComponent* TurretHead;  
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret")  
	UStaticMeshComponent* TurretBody;  
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret")  
	UStaticMeshComponent* Turretneck;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    USceneComponent* MuzzleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret")  
	UStaticMeshComponent* MuzzleLocation; // 총구 위치를 나타낼 컴포넌트  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret AI")  
	bool bIsScanning = true;  
};
