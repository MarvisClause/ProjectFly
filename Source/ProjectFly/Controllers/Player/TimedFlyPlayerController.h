#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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
class PROJECTFLY_API ATimedFlyPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    virtual void SetupInputComponent() override;

    void SetUIState(EUIState NewState);

private:
    UFUNCTION()
    void HandleRunFinished(EFlyResult FlyResult, float TimeInSeconds);

    UFUNCTION()
    void TogglePause();

    UPROPERTY()
    TObjectPtr<AGliderPawn> GliderPawn;

    UPROPERTY()
    TObjectPtr<ATimedFlyGameMode> TimedFlyGameMode;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UTimedFlyHUDWidget> HUDClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UTimedFlyResultsWidget> ResultsClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UTimedFlyPauseWidget> PauseClass;

    UPROPERTY()
    TObjectPtr<UTimedFlyHUDWidget> HUDWidget;

    UPROPERTY()
    TObjectPtr<UTimedFlyResultsWidget> ResultsWidget;

    UPROPERTY()
    TObjectPtr<UTimedFlyPauseWidget> PauseWidget;
};