#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimedFlyHUDWidget.generated.h"

class UTextBlock;

UCLASS(Abstract)
class PROJECTFLY_API UTimedFlyHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetTime(float TimeInSeconds);

    void SetHealth(float Health);

    void SetSpeed(float Speed);

protected:
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> TimeText;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> HealthText;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> SpeedText;
};