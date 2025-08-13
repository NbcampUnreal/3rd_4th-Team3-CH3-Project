// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "AZombieCharacter.generated.h"

class UHealthComponent;

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


    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:
    //임시 추가
    //공통hp (hp바 안씀)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
    UHealthComponent* HealthComp = nullptr;

    //각 클래스(일반/특수)좀비에서 원하는 기본 MAXHP 세팅가능
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "1"))
    float DefaultMaxHealth = 100.f; //일반좀비 기본값100
    //임시추가 여기까지
  
private:
    
};
