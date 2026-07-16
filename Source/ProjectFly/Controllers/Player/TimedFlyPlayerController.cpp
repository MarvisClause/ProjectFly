#include "TimedFlyPlayerController.h"
#include "ProjectFly/Pawns/GliderPawn.h"
#include "ProjectFly/GameModes/TimedFlyGameMode.h"
#include "ProjectFly/Components/HealthComponent.h"
#include "ProjectFly/Components/FlightPhysicsComponent.h"
#include "ProjectFly/UI/TimedFly/TimedFlyHUDWidget.h"
#include "ProjectFly/UI/TimedFly/TimedFlyPauseWidget.h"
#include "ProjectFly/UI/TimedFly/TimedFlyResultsWidget.h"

void ATimedFlyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Get reference to game mode
    TimedFlyGameMode = GetWorld()->GetAuthGameMode<ATimedFlyGameMode>();
    if (TimedFlyGameMode)
    {
        TimedFlyGameMode->OnFlyEnded.AddDynamic(this, &ATimedFlyPlayerController::HandleRunFinished);
    }

    // Initialize widgets
    PauseWidget = CreateWidget<UTimedFlyPauseWidget>(this, PauseClass);
    ResultsWidget = CreateWidget<UTimedFlyResultsWidget>(this, ResultsClass);

    PauseWidget->AddToViewport();
    ResultsWidget->AddToViewport();

    PauseWidget->SetVisibility(ESlateVisibility::Hidden);
    ResultsWidget->SetVisibility(ESlateVisibility::Hidden);

    PauseWidget->OnResumeButtonAction.AddDynamic(this, &ATimedFlyPlayerController::TogglePause);

    SetUIState(EUIState::Gameplay);

    // Cast HUD widget
    TimedHUD = Cast<UTimedFlyHUDWidget>(HUDWidget);
}

void ATimedFlyPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (TimedHUD)
    {
        TimedHUD->SetTime(TimedFlyGameMode->GetCurrentTimeInSeconds());
    }
}

void ATimedFlyPlayerController::SetUIState(EUIState NewState)
{
    HUDWidget->SetVisibility(ESlateVisibility::Hidden);
    PauseWidget->SetVisibility(ESlateVisibility::Hidden);
    ResultsWidget->SetVisibility(ESlateVisibility::Hidden);

    switch (NewState)
    {
    case EUIState::Gameplay:
        HUDWidget->SetVisibility(ESlateVisibility::Visible);

        SetPause(false);
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;

        EnableGameplayInput();

        break;

    case EUIState::Pause:
        PauseWidget->SetVisibility(ESlateVisibility::Visible);

        PauseWidget->SetResults(TimedFlyGameMode->GetCurrentTimeInSeconds());

        SetPause(true);
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;

        DisableGameplayInput();

        break;

    case EUIState::Results:
        ResultsWidget->SetVisibility(ESlateVisibility::Visible);

        SetPause(false);
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;

        DisableGameplayInput();

        break;
    }
}

void ATimedFlyPlayerController::HandleRunFinished(EFlyResult FlyResult, float TimeInSeconds)
{
    ResultsWidget->SetResults(FlyResult == EFlyResult::Finished, TimeInSeconds);
    SetUIState(EUIState::Results);
}

void ATimedFlyPlayerController::TogglePause()
{
    if (!ResultsWidget->IsVisible())
    {
        PauseWidget->IsVisible() ? SetUIState(EUIState::Gameplay) : SetUIState(EUIState::Pause);
    }
}