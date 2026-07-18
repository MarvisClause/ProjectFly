#include "MainMenuPlayerController.h"
#include "ProjectFly/UI/MainMenu/MainMenuUserWidget.h"
#include "ProjectFly/Pawns/GliderPawn.h"
#include "ProjectFly/Components/FlightPhysicsComponent.h"
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

void AMainMenuPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Ensure menu glider exists
    if (!IsValid(MenuGliderPawn))
    {
        return;
    }

    // Deproject mouse cursor into the world
    FVector MouseWorldLocation;
    FVector MouseWorldDirection;

    if (!DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
    {
        return;
    }

    // Get active camera information
    check(PlayerCameraManager);

    const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
    const FVector CameraForward = PlayerCameraManager->GetActorForwardVector();

    // Create plane in front of the camera, where mouse cursor will be projected
    const FVector ProjectionPlaneOrigin = CameraLocation + CameraForward * CursorProjectionDistance;
    const FPlane ProjectionPlane(ProjectionPlaneOrigin, CameraForward);

    // Trace ray from the mouse onto the projection plane
    constexpr float MouseRayLength = 100000.0f;
    const FVector MouseRayEnd = MouseWorldLocation + MouseWorldDirection * MouseRayLength;
    const FVector CursorWorldPosition = FMath::LinePlaneIntersection(MouseWorldLocation, MouseRayEnd, ProjectionPlane);

    // Set autopilot control
    MenuGliderPawn->SetAutopilotControl(false);
    // Update glider autopilot target
    MenuGliderPawn->AccessFlightPhysicsComponent()->SetTargetAutopilotPosition(CursorWorldPosition);
}