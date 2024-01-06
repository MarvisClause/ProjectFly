// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

class UMainMenuUserWidget;

/**
 * HUD for control of the main menu
 */
UCLASS(Abstract)
class PROJECTFLY_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenuUserWidget> MainMenuUserWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UMainMenuUserWidget> MainMenuUserWidget;
};
