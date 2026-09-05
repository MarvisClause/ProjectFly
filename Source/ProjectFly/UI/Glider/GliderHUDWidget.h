#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GliderHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UHintMessageWidget;

UCLASS(Abstract)
class PROJECTFLY_API UGliderHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetHealth(float Health);

    void SetSpeed(float Speed);

    void SetStamina(float Stamina);

    void SetDashCharge(float Percent);

    void SetHintMessage(FText Message, float DisplayTime);

protected:
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> HealthText;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> SpeedText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> StaminaText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DashChargeText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHintMessageWidget> HintMessageWidget;
};