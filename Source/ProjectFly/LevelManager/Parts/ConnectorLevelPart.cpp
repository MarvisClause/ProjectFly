#include "ProjectFly/LevelManager/Parts/ConnectorLevelPart.h"
#include "TimerManager.h"

// Sets default values
AConnectorLevelPart::AConnectorLevelPart()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = false;

    // Create components
    GatewayTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("GatewayTrigger"));
    RootComponent = GatewayTrigger;

    EnterGatewayBlock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnterGatewayBlock"));
    EnterGatewayBlock->SetupAttachment(RootComponent);

    ExitGatewayBlock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExitGatewayBlock"));
    ExitGatewayBlock->SetupAttachment(RootComponent);

    // Bind the handler function to the gateway trigger
    GatewayTrigger->OnComponentBeginOverlap.AddDynamic(this, &AConnectorLevelPart::HandleGatewayEnter);

    // Initialize default values
    CloseDelay = 5.0f; // Default delay before closing the gateway
    bIsGatewayOpen = true; // The gateway starts as open
}

// Called when the game starts or when spawned
void AConnectorLevelPart::BeginPlay()
{
    Super::BeginPlay();

    // Additional initialization logic can be added here
}

// Function to handle logic when something enters the gateway
void AConnectorLevelPart::HandleGatewayEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bIsGatewayOpen)
    {
        // Trigger the event when the gateway is entered
        OnGatewayTriggerEnter(OtherActor);

        // Close the gateway after a delay
        GetWorldTimerManager().SetTimerForNextTick(this, &AConnectorLevelPart::CloseGateway);

        // Notify upper managers about entering the connector
        NotifyUpperManagers();

        bIsGatewayOpen = false; // Close the gateway
    }
}

// Function to close the gateway after a delay
void AConnectorLevelPart::CloseGateway()
{
    EnterGatewayBlock->SetVisibility(false);
    ExitGatewayBlock->SetVisibility(true);
}

// Function to notify upper managers about entering the connector
void AConnectorLevelPart::NotifyUpperManagers()
{
    // Logic to notify upper managers about entering the connector
    // This could involve triggering level generation or biome switching events.
}