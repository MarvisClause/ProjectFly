// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectFly/UI/MainMenu/MainMenuUserWidget.h"
#include "Components/Button.h"
#include <Kismet/GameplayStatics.h>

void UMainMenuUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    check(StartGameButton);
    check(QuitGameButton);

    StartGameButton->OnClicked.AddDynamic(this, &UMainMenuUserWidget::StartGameButtonAction);
    QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuUserWidget::QuitGameButtonAction);
}

void UMainMenuUserWidget::StartGameButtonAction()
{
    UGameplayStatics::OpenLevel(this, GameLevelName, false);
}

void UMainMenuUserWidget::QuitGameButtonAction()
{
    FGenericPlatformMisc::RequestExit(false);
}