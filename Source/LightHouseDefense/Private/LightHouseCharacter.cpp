// Fill out your copyright notice in the Description page of Project Settings.


#include "LightHouseCharacter.h"
#include "HealthComponent.h"      // HealthComp 생성/사용
#include "HealthSubsystem.h"      // 초기 HP 세팅 호출
#include "Engine/World.h"
#include "Engine/GameInstance.h"  // GetGameInstance()


// Sets default values
ALightHouseCharacter::ALightHouseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

     
  // FIX: 플레이어 체력 컴포넌트 생성 및 부착
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    // 플레이어에선 보통 죽어도 액터를 파괴하지 않음
    HealthComponent->bDestroyOwnerOnDeath = false;
    
}

// Called when the game starts or when spawned
void ALightHouseCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UHealthSubsystem* HS = GI->GetSubsystem<UHealthSubsystem>())
        {
            HS->InitializeHealthForActor(this);
        }
    }
}

// Called every frame
void ALightHouseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALightHouseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ALightHouseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // C++ Log
    UE_LOG(LogTemp, Warning, TEXT("ALightHouseCharacter::TakeDamage Called! Damage: %f"), DamageAmount);

   
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
