#include "MainMenuPlayerController.h"
#include "ProjectFly/UI/MainMenu/MainMenuUserWidget.h"

void AMainMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    check(MainMenuWidgetClass);

    MainMenuWidget = CreateWidget<UMainMenuUserWidget>(this, MainMenuWidgetClass);
    check(MainMenuWidget);

    MainMenuWidget->AddToViewport();

    bShowMouseCursor = true;

    SetInputMode(FInputModeUIOnly());
}