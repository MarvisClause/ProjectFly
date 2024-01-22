#include "ProjectFly/LevelManager/Parts/BaseLevelPart.h"

// Sets default values
ABaseLevelPart::ABaseLevelPart()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = false;

    // Create and attach floor start connector
    FloorStartConnector = CreateDefaultSubobject<USceneComponent>(TEXT("FloorStartConnector"));
    RootComponent = FloorStartConnector;

    // Create and attach floor end connector
    FloorEndConnector = CreateDefaultSubobject<USceneComponent>(TEXT("FloorEndConnector"));
    FloorEndConnector->SetupAttachment(RootComponent);

    // Create and attach floor mesh
    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
    FloorMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ABaseLevelPart::BeginPlay()
{
    Super::BeginPlay();
}

// Returns the start connector component
USceneComponent* ABaseLevelPart::GetFloorStartConnector() const
{
    return FloorStartConnector;
}

// Returns the end connector component
USceneComponent* ABaseLevelPart::GetFloorEndConnector() const
{
    return FloorEndConnector;
}

// Attaches this level part to another ABaseLevelPart via their connector components
void ABaseLevelPart::AttachToLevelPart(ABaseLevelPart* OtherLevelPart)
{
    if (OtherLevelPart)
    {
        //logic to attach the level parts using their connector components
        
    }
}