// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LightHouseCharacter.generated.h"


class UHealthComponent; // HealthComponent 전방 선언 (cpp에서 include)

UCLASS()
class LIGHTHOUSEDEFENSE_API ALightHouseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALightHouseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
    // FIX: 플레이어 체력 컴포넌트(HP/피해 처리/사망 브로드캐스트)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    UHealthComponent* HealthComponent;
private:


};
