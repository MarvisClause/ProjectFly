#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimedFlyPauseWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResumeButtonAction);

// Pause
UCLASS(Abstract)
class PROJECTFLY_API UTimedFlyPauseWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnResumeButtonAction OnResumeButtonAction;

protected:
    virtual void NativeConstruct() override;

    virtual void NativeDestruct() override;

    void SetResults(float TimeInSeconds);

    // Result time
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UTextBlock> ResultsTimeText;

    // Resume game. It will fire an event, which allows owner to define behavior
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> ResumeButton;

    // Restart level
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> RestartLevelButton;

    // Return to main menu
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> ReturnToMainMenuButton;

private:
    UFUNCTION()
    void ResumeAction();

    UFUNCTION()
    void RestartLevelAction();

    UFUNCTION()
    void ReturnToMainMenuAction();
};
