#include "TimedFlyResultsWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>
#include "ProjectFly/GameInstance/ProjectFlyGameInstance.h"

void UTimedFlyResultsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    check(ResultsTimeText);
    check(NextLevelButton);
    check(RestartLevelButton);
    check(ReturnToMainMenuButton);

    NextLevelButton->OnClicked.AddDynamic(this, &UTimedFlyResultsWidget::LoadNextLevelAction);
    RestartLevelButton->OnClicked.AddDynamic(this, &UTimedFlyResultsWidget::RestartLevelAction);
    ReturnToMainMenuButton->OnClicked.AddDynamic(this, &UTimedFlyResultsWidget::ReturnToMainMenuAction);
}

void UTimedFlyResultsWidget::NativeDestruct()
{
    if (NextLevelButton)
    {
        NextLevelButton->OnClicked.RemoveDynamic(this, &UTimedFlyResultsWidget::LoadNextLevelAction);
    }

    if (RestartLevelButton)
    {
        RestartLevelButton->OnClicked.RemoveDynamic(this, &UTimedFlyResultsWidget::RestartLevelAction);
    }

    if (ReturnToMainMenuButton)
    {
        ReturnToMainMenuButton->OnClicked.RemoveDynamic(this, &UTimedFlyResultsWidget::ReturnToMainMenuAction);
    }

    Super::NativeDestruct();
}

void UTimedFlyResultsWidget::SetResults(bool bIsFinished, float TimeInSeconds)
{
    // Save success state for UI
    NotifyTimedFlySuccess(bIsFinished);

    // Define next level button visibility
    NextLevelButton->SetVisibility(bIsFinished ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    // Don't show next level button, if we reached final level of the game
    if (UProjectFlyGameInstance* GameInstance = Cast<UProjectFlyGameInstance>(GetGameInstance()))
    {
        if (GameInstance->IsCurrentLevelLast())
        {
            NextLevelButton->SetVisibility( ESlateVisibility::Hidden);
        }

        if (bIsFinished)
        {
            GameInstance->CompleteCurrentLevel();
        }
    }

    // Set time
    const int32 TotalSeconds = FMath::FloorToInt(TimeInSeconds);
    const int32 Minutes = TotalSeconds / 60;
    const int32 Seconds = TotalSeconds % 60;
    ResultsTimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
}

void UTimedFlyResultsWidget::LoadNextLevelAction()
{
    // Load next level
    if (UProjectFlyGameInstance* GameInstance = Cast<UProjectFlyGameInstance>(GetGameInstance()))
    {
        GameInstance->LoadNextLevel();
    }
}

void UTimedFlyResultsWidget::RestartLevelAction()
{
    // Restart level
    if (UProjectFlyGameInstance* GameInstance = Cast<UProjectFlyGameInstance>(GetGameInstance()))
    {
        GameInstance->RestartLevel();
    }
}

void UTimedFlyResultsWidget::ReturnToMainMenuAction()
{
    // Load main menu level
    if (UProjectFlyGameInstance* GameInstance = Cast<UProjectFlyGameInstance>(GetGameInstance()))
    {
        GameInstance->LoadMainMenuLevel();
    }
}