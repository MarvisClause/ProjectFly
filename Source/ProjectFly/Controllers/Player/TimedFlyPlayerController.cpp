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

    // Get reference to glider pawn
    GliderPawn = Cast<AGliderPawn>(GetPawn());

    // Get reference to game mode
    TimedFlyGameMode = GetWorld()->GetAuthGameMode<ATimedFlyGameMode>();
    if (TimedFlyGameMode)
    {
        TimedFlyGameMode->OnFlyEnded.AddDynamic(this, &ATimedFlyPlayerController::HandleRunFinished);
    }

    // Initialize widgets
    HUDWidget = CreateWidget<UTimedFlyHUDWidget>(this, HUDClass);
    PauseWidget = CreateWidget<UTimedFlyPauseWidget>(this, PauseClass);
    ResultsWidget = CreateWidget<UTimedFlyResultsWidget>(this, ResultsClass);

    HUDWidget->AddToViewport();
    PauseWidget->AddToViewport();
    ResultsWidget->AddToViewport();

    HUDWidget->SetVisibility(ESlateVisibility::Visible);
    PauseWidget->SetVisibility(ESlateVisibility::Hidden);
    ResultsWidget->SetVisibility(ESlateVisibility::Hidden);

    PauseWidget->OnResumeButtonAction.AddDynamic(this, &ATimedFlyPlayerController::TogglePause);

    SetUIState(EUIState::Gameplay);
}

void ATimedFlyPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    HUDWidget->SetHealth(GliderPawn->AccessHealthComponent()->CurrentHealth);

    HUDWidget->SetSpeed(GliderPawn->AccessFlightPhysicsComponent()->GetForwardSpeed());

    HUDWidget->SetTime(TimedFlyGameMode->GetCurrentTimeInSeconds());
}

void ATimedFlyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction( "Pause", IE_Pressed, this, &ATimedFlyPlayerController::TogglePause);
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
        break;

    case EUIState::Pause:
        PauseWidget->SetVisibility(ESlateVisibility::Visible);

        SetPause(true);
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;
        break;

    case EUIState::Results:
        ResultsWidget->SetVisibility(ESlateVisibility::Visible);

        SetPause(true);
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;
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