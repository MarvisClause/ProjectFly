// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUserWidget.generated.h"

class UButton;

/**
 * Controls main menu elements
 */
UCLASS(Abstract)
class PROJECTFLY_API UMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

    // Name of the game level
    UPROPERTY(EditDefaultsOnly)
    FName GameLevelName;

    // Score count
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> StartGameButton;

    // Target count
    UPROPERTY(Transient, meta = (BindWidget))
    TObjectPtr<UButton> QuitGameButton;

private:
    UFUNCTION()
    void StartGameButtonAction();

    UFUNCTION()
    void QuitGameButtonAction();
};
