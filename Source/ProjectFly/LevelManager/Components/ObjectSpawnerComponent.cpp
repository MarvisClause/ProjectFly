#include "ObjectSpawnerComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UObjectSpawnerComponent::UObjectSpawnerComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UObjectSpawnerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UObjectSpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    DestroySpawnedObject();
}

// Spawns an obstacle with probability-based selection
void UObjectSpawnerComponent::SpawnObstacle()
{
    // Implement spawning logic for obstacles with probability
    if (ObstaclesToSpawn.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, ObstaclesToSpawn.Num() - 1);
        SpawnObject(ObstaclesToSpawn[RandomIndex].ObjectClass);
    }
}

// Spawns a booster with probability-based selection
void UObjectSpawnerComponent::SpawnBooster()
{
    // Implement spawning logic for boosters with probability
    if (BoostersToSpawn.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, BoostersToSpawn.Num() - 1);
        SpawnObject(BoostersToSpawn[RandomIndex].ObjectClass);
    }
}

// Destroys the currently spawned object
void UObjectSpawnerComponent::DestroySpawnedObject()
{
    if (SpawnedObject)
    {
        SpawnedObject->Destroy();
        SpawnedObject = nullptr;
    }
}

// Spawns an object based on spawn data
void UObjectSpawnerComponent::SpawnObject(const TSubclassOf<AActor> SpawnObject)
{
    // Implement spawning logic using SpawnObject
    if (SpawnObject)
    {
        SpawnedObject = GetWorld()->SpawnActor<AActor>(SpawnObject, GetComponentLocation(), FRotator::ZeroRotator);
    }
}