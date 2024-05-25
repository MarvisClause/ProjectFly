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
        SpawnObject(ObstaclesToSpawn[RandomIndex]);
    }
}

// Spawns a booster with probability-based selection
void UObjectSpawnerComponent::SpawnBooster()
{
    // Implement spawning logic for boosters with probability
    if (BoostersToSpawn.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, BoostersToSpawn.Num() - 1);
        SpawnObject(BoostersToSpawn[RandomIndex]);
    }
}

// Destroys the currently spawned object
void UObjectSpawnerComponent::DestroySpawnedObject()
{
    if (IsValid(SpawnedObject))
    {
        SpawnedObject->Destroy();
        SpawnedObject = nullptr;
    }
}

// Spawns an object based on spawn data
void UObjectSpawnerComponent::SpawnObject(const FSpawnObjectData& SpawnObject)
{
    if (!IsValid(SpawnObject.ObjectClass))
    { 
        ensureMsgf(IsValid(SpawnObject.ObjectClass), TEXT("Object class was not defined for object spawner data!"));
        return;
    }

    // Implement spawning logic using SpawnObject
    if (FMath::RandRange(1, 100) <= SpawnObject.SpawnChance)
    {
        SpawnedObject = GetWorld()->SpawnActor<AActor>(SpawnObject.ObjectClass, GetComponentLocation(), FRotator::ZeroRotator);
    }
}