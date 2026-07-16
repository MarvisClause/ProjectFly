#pragma once

#include "CoreMinimal.h"
#include "ProjectFly/Controllers/Player/GliderPlayerController.h"
#include "ProjectFly/GameModes/TimedFlyGameMode.h"
#include "TimedFlyPlayerController.generated.h"

class AGliderPawn;
class ATimedFlyGameMode;
class UTimedFlyHUDWidget;
class UTimedFlyResultsWidget;
class UTimedFlyPauseWidget;

UENUM()
enum class EUIState : uint8
{
    Gameplay,
    Pause,
    Results
};

UCLASS()
class PROJECTFLY_API ATimedFlyPlayerController : public AGliderPlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    void SetUIState(EUIState NewState);

private:
    UFUNCTION()
    void HandleRunFinished(EFlyResult FlyResult, float TimeInSeconds);

    UFUNCTION()
    virtual void TogglePause() override;

    UPROPERTY()
    TObjectPtr<ATimedFlyGameMode> TimedFlyGameMode;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UTimedFlyResultsWidget> ResultsClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UTimedFlyPauseWidget> PauseClass;

    UPROPERTY()
    TObjectPtr<UTimedFlyResultsWidget> ResultsWidget;

    UPROPERTY()
    TObjectPtr<UTimedFlyPauseWidget> PauseWidget;

    // Pointer to the base HUD widget with attempt to cast it to according child widget
    UPROPERTY()
    UTimedFlyHUDWidget* TimedHUD;
};