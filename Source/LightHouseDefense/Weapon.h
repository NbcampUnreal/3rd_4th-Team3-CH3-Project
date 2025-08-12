// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
    //발사
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();

    // BP에서 총구 화염/사운드 구현용
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void PlayFireEffect();

    //장착/해제 API
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Equip(class APawn* NewOwnerPawn, FName SocketName = "hand_r_socket"); // ★ 추가

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Unequip(); 


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeaponProjectile> ProjectileClass; // 발사할 총알 클래스

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Range = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate = 1.0f;  // 초당 발사 빈도

    //총구 위치용 머즐
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    class USceneComponent* Muzzle = nullptr;

    //탄창등 소켓 이름 캐시
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Weapon")
    FName EquippedSocketName = NAME_None;






};



