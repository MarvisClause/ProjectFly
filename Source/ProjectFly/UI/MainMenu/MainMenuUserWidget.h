#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUserWidget.generated.h"

class UButton;

// Controls main menu elements 
UCLASS(Abstract)
class PROJECTFLY_API UMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

    virtual void NativeDestruct() override;

    // Name of the game level
    UPROPERTY(EditDefaultsOnly, Category="Main Menu")
    TSoftObjectPtr<UWorld> GameLevel;

    // Continue game button
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> ContinueGameButton;

    // New game button
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> NewGameButton;

    // Quit game button
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> QuitGameButton;

private:
    UFUNCTION()
    void ContinueGameButtonAction();

    UFUNCTION()
    void NewGameButtonAction();

    UFUNCTION()
    void QuitGameButtonAction();
};
