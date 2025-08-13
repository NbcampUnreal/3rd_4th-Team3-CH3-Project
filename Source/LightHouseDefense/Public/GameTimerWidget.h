#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameTimerWidget.generated.h"


class UTextBlock;


UCLASS()
class UGameTimerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    void UpdateTimeText(int32 RemainingSeconds);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimerText;
};
