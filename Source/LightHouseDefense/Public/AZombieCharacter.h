// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AZombieCharacter.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API AAZombieCharacter : public ACharacter 
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAZombieCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    class USkeletalMeshComponent* ZombieMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    TSubclassOf<class AAIController> ZombieAIControllerClass;

private:
    
};
