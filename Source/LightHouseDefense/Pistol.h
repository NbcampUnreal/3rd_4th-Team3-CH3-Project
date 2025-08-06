// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pistol.generated.h"

UCLASS()
class LIGHTHOUSEDEFENSE_API APistol : public AActor
{
    GENERATED_BODY()

public:
    APistol();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // 총 발사 함수
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Fire();

    // 총알 발사 시 이펙트 
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void PlayFireEffect();

    // 총알 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 25.0f;

    // 사거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Range = 1000.0f;
};
