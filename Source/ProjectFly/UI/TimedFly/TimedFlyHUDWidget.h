#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectFly/UI/Glider/GliderHUDWidget.h"
#include "TimedFlyHUDWidget.generated.h"

class UTextBlock;

UCLASS(Abstract)
class PROJECTFLY_API UTimedFlyHUDWidget : public UGliderHUDWidget
{
    GENERATED_BODY()

public:
    void SetTime(float TimeInSeconds);

protected:
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> TimeText;
};