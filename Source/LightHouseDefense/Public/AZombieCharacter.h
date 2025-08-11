// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "AZombieCharacter.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API AAZombieCharacter : public ACharacter, public IGenericTeamAgentInterface // <-- 여기에 public IGenericTeamAgentInterface를 추가해야 합니다.
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

    virtual FGenericTeamId GetGenericTeamId() const override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    class USkeletalMeshComponent* ZombieMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    TSubclassOf<class AAIController> ZombieAIControllerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FGenericTeamId TeamID;

private:
    
};
