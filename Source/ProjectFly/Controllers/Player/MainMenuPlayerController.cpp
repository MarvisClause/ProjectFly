#include "MainMenuPlayerController.h"
#include "ProjectFly/UI/MainMenu/MainMenuUserWidget.h"
#include "ProjectFly/Pawns/GliderPawn.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Initialize widgets
    check(MainMenuWidgetClass);

    MainMenuWidget = CreateWidget<UMainMenuUserWidget>(this, MainMenuWidgetClass);
    check(MainMenuWidget);

    MainMenuWidget->AddToViewport();

    // Initialize input
    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    SetInputMode(InputMode);

    bShowMouseCursor = true;

    // Get menu glider pawn
    MenuGliderPawn = Cast<AGliderPawn>( UGameplayStatics::GetActorOfClass( this, AGliderPawn::StaticClass()));
}