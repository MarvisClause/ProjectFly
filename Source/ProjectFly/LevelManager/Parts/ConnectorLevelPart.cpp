#include "ProjectFly/LevelManager/Parts/ConnectorLevelPart.h"
#include "TimerManager.h"
#include "ProjectFly/Pawns/BaseFlyPlane.h"

// Sets default values
AConnectorLevelPart::AConnectorLevelPart()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = false;

    // Create components
    GatewayTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("GatewayTrigger"));
    GatewayTrigger->SetupAttachment(RootComponent);

    EnterGatewayBlock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnterGatewayBlock"));
    EnterGatewayBlock->SetupAttachment(RootComponent);

    ExitGatewayBlock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExitGatewayBlock"));
    ExitGatewayBlock->SetupAttachment(RootComponent);

    // Initialize default values
    CloseDelay = 5.0f; // Default delay before closing the gateway
    bIsGatewayOpen = true; // The gateway starts as open
}

// Called when the game starts or when spawned
void AConnectorLevelPart::BeginPlay()
{
    Super::BeginPlay();

    // Additional initialization logic can be added here
    OpenGateway();

    // Bind the handler function to the gateway trigger
    GatewayTrigger->OnComponentBeginOverlap.AddDynamic(this, &AConnectorLevelPart::HandleGatewayEnter);
}

// Function to handle logic when something enters the gateway
void AConnectorLevelPart::HandleGatewayEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(ABaseFlyPlane::StaticClass()) && bIsGatewayOpen)
    {
        // Close the gateway after a delay
        GetWorldTimerManager().SetTimerForNextTick(this, &AConnectorLevelPart::CloseGateway);

        // Notify upper managers about entering the connector
        NotifyUpperManagers();
    }
}

void AConnectorLevelPart::OpenGateway()
{
    EnterGatewayBlock->SetVisibility(false);
    EnterGatewayBlock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    ExitGatewayBlock->SetVisibility(true);
    ExitGatewayBlock->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Function to close the gateway after a delay
void AConnectorLevelPart::CloseGateway()
{
    EnterGatewayBlock->SetVisibility(true);
    EnterGatewayBlock->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    
    ExitGatewayBlock->SetVisibility(false);
    ExitGatewayBlock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Function to notify upper managers about entering the connector
void AConnectorLevelPart::NotifyUpperManagers()
{
    // Logic to notify upper managers about entering the connector
    // This could involve triggering level generation or biome switching events.
    // Trigger the event when the gateway is entered
    OnGatewayEnterDelegate.Broadcast(this);
    bIsGatewayOpen = false;
}