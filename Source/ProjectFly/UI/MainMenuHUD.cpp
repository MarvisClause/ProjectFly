// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectFly/UI/MainMenuHUD.h"
#include "ProjectFly/UI/MainMenu/MainMenuUserWidget.h"

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

    check(MainMenuUserWidgetClass);
    MainMenuUserWidget = CreateWidget<UMainMenuUserWidget>(GetWorld(), MainMenuUserWidgetClass);
    check(MainMenuUserWidget);
    MainMenuUserWidget->AddToViewport();
}