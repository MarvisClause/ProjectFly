#include "MainMenuUserWidget.h"
#include "Components/Button.h"
#include <Kismet/GameplayStatics.h>
#include "ProjectFly/GameInstance/ProjectFlyGameInstance.h"

void UMainMenuUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    check(ContinueGameButton);
    check(NewGameButton);
    check(QuitGameButton);

    ContinueGameButton->OnClicked.AddDynamic(this, &UMainMenuUserWidget::ContinueGameButtonAction);
    NewGameButton->OnClicked.AddDynamic(this, &UMainMenuUserWidget::NewGameButtonAction);
    QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuUserWidget::QuitGameButtonAction);

    if (UProjectFlyGameInstance* GameInstance = Cast<UProjectFlyGameInstance>(GetGameInstance()))
    {
        // If current highest level is zero, that means, that user didn't
        if (GameInstance->HighestUnlockedLevel == 0)
        {
            ContinueGameButton->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UMainMenuUserWidget::NativeDestruct()
{
    if (ContinueGameButton)
    {
        ContinueGameButton->OnClicked.RemoveDynamic(this, &UMainMenuUserWidget::ContinueGameButtonAction);
    }

    if (NewGameButton)
    {
        NewGameButton->OnClicked.RemoveDynamic(this, &UMainMenuUserWidget::NewGameButtonAction);
    }

    if (QuitGameButton)
    {
        QuitGameButton->OnClicked.RemoveDynamic(this, &UMainMenuUserWidget::QuitGameButtonAction);
    }

    Super::NativeDestruct();
}

void UMainMenuUserWidget::ContinueGameButtonAction()
{
    if (UProjectFlyGameInstance* GameInstance = Cast<UProjectFlyGameInstance>(GetGameInstance()))
    {
        GameInstance->LoadCurrentLevel();
    }
}

void UMainMenuUserWidget::NewGameButtonAction()
{
    if (UProjectFlyGameInstance* GameInstance = Cast<UProjectFlyGameInstance>(GetGameInstance()))
    {
        GameInstance->StartNewGame();
    }
}

void UMainMenuUserWidget::QuitGameButtonAction()
{
    FGenericPlatformMisc::RequestExit(false);
}