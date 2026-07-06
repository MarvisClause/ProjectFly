#include "TimedFlyPauseWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>
#include "ProjectFly/GameInstance/ProjectFlyGameInstance.h"

void UTimedFlyPauseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    check(ResultsTimeText);
    check(ResumeButton);
    check(RestartLevelButton);
    check(ReturnToMainMenuButton);

    ResumeButton->OnClicked.AddDynamic(this, &UTimedFlyPauseWidget::ResumeAction);
    RestartLevelButton->OnClicked.AddDynamic(this, &UTimedFlyPauseWidget::RestartLevelAction);
    ReturnToMainMenuButton->OnClicked.AddDynamic(this, &UTimedFlyPauseWidget::ReturnToMainMenuAction);
}

void UTimedFlyPauseWidget::NativeDestruct()
{
    if (ResumeButton)
    {
        ResumeButton->OnClicked.RemoveDynamic(this, &UTimedFlyPauseWidget::ResumeAction);
    }

    if (RestartLevelButton)
    {
        RestartLevelButton->OnClicked.RemoveDynamic(this, &UTimedFlyPauseWidget::RestartLevelAction);
    }

    if (ReturnToMainMenuButton)
    {
        ReturnToMainMenuButton->OnClicked.RemoveDynamic(this, &UTimedFlyPauseWidget::ReturnToMainMenuAction);
    }

    Super::NativeDestruct();
}

void UTimedFlyPauseWidget::SetResults(float TimeInSeconds)
{
    // Set time
    const int32 TotalSeconds = FMath::FloorToInt(TimeInSeconds);
    const int32 Minutes = TotalSeconds / 60;
    const int32 Seconds = TotalSeconds % 60;
    ResultsTimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
}

void UTimedFlyPauseWidget::ResumeAction()
{
    OnResumeButtonAction.Broadcast();
}

void UTimedFlyPauseWidget::RestartLevelAction()
{
    // Restart level
    if (UProjectFlyGameInstance* GameInstance = Cast<UProjectFlyGameInstance>(GetGameInstance()))
    {
        GameInstance->RestartLevel();
    }
}

void UTimedFlyPauseWidget::ReturnToMainMenuAction()
{
    // Load main menu level
    if (UProjectFlyGameInstance* GameInstance = Cast<UProjectFlyGameInstance>(GetGameInstance()))
    {
        GameInstance->LoadMainMenuLevel();
    }
}