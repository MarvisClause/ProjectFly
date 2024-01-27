#include "ProjectFly/LevelManager/Parts/BaseLevelPart.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseLevelPart, Log, Log);

// Sets default values
ABaseLevelPart::ABaseLevelPart()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = false;

    // Create and attach floor mesh
    BaseFloorSceneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseFloorSceneComponent"));
    RootComponent = BaseFloorSceneComponent;

    // Create and attach floor mesh
    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
    FloorMesh->SetupAttachment(RootComponent);

    // Create and attach floor end connector
    FloorEndConnector = CreateDefaultSubobject<USceneComponent>(TEXT("FloorEndConnector"));
    FloorEndConnector->SetupAttachment(FloorMesh);

    // Create and attach floor start connector
    FloorStartConnector = CreateDefaultSubobject<USceneComponent>(TEXT("FloorStartConnector"));
    FloorStartConnector->SetupAttachment(FloorMesh);

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
        UE_LOG(LogBaseLevelPart, Log, TEXT("Floor %s attached to %s"), *GetName(), *OtherLevelPart->GetName());

        // Set floor location and rotation according to other floor end connector data
        SetActorRotation(OtherLevelPart->GetFloorEndConnector()->GetComponentRotation());
        SetActorLocation(OtherLevelPart->GetFloorEndConnector()->GetComponentLocation());
        // Offset floor, so start connect is located on actor location
        SetLocationByStartConnector(GetActorLocation());
    }
}

void ABaseLevelPart::SetLocationByStartConnector(const FVector& Location)
{
    // Move floor in the way that start connector location is at actor location
    SetActorLocation(Location + (Location - GetFloorStartConnector()->GetComponentLocation()));
}