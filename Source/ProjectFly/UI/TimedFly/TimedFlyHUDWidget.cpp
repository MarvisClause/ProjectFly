#include "TimedFlyHUDWidget.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>

void UTimedFlyHUDWidget::SetTime(float TimeInSeconds)
{
    const int32 TotalSeconds = FMath::FloorToInt(TimeInSeconds);
    const int32 Minutes = TotalSeconds / 60;
    const int32 Seconds = TotalSeconds % 60;
    TimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
}