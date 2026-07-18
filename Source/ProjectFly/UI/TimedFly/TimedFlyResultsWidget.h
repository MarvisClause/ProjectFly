#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimedFlyResultsWidget.generated.h"

class UTextBlock;
class UButton;

// Depicts result of timed fly
UCLASS(Abstract)
class PROJECTFLY_API UTimedFlyResultsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetResults(bool bIsFinished, float TimeInSeconds);

protected:
    virtual void NativeConstruct() override;

    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintImplementableEvent)
    void NotifyTimedFlySuccess(bool bIsSuccess);

    // Result time
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UTextBlock> ResultsTimeText;

    // Next level
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> NextLevelButton;

    // Restart level
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> RestartLevelButton;

    // Return to main menu
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> ReturnToMainMenuButton;

private:
    UFUNCTION()
    void LoadNextLevelAction();

    UFUNCTION()
    void RestartLevelAction();

    UFUNCTION()
    void ReturnToMainMenuAction();
};
