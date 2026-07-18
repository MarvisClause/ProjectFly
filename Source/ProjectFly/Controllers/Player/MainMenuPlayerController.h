#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UMainMenuUserWidget;
class AGliderPawn;

UCLASS()
class PROJECTFLY_API AMainMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditDefaultsOnly, Category = "UI - Glider Plane")
    float CursorProjectionDistance = 3000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMainMenuUserWidget> MainMenuWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UMainMenuUserWidget> MainMenuWidget;

    // Menu plane follows cursor position. Consider this just as a visual gimmic
    UPROPERTY()
    TObjectPtr<AGliderPawn> MenuGliderPawn;
};