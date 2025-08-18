#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartRunVolume.generated.h"

class UBoxComponent;

UCLASS()
class LIGHTHOUSEDEFENSE_API AStartRunVolume : public AActor
{
    GENERATED_BODY()

public:
    AStartRunVolume();

    // 에디터에서 사운드 에셋을 할당할 변수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    class USoundBase* BackgroundMusic;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere) UBoxComponent* Box = nullptr;

    UFUNCTION() void OnBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep);
    UFUNCTION() void OnEnd(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex);

    void HandleStartPressed(); // F키 핸들러

private:
    bool bPlayerInside = false;

    UPROPERTY()
    class UAudioComponent* MusicComponent;

};
