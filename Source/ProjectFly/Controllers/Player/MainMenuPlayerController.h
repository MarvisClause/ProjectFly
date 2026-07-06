#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UMainMenuUserWidget;

UCLASS()
class PROJECTFLY_API AMainMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMainMenuUserWidget> MainMenuWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UMainMenuUserWidget> MainMenuWidget;
};