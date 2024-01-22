#include "ProjectFly/LevelManager/Parts/GameLevelPart.h"

// Sets default values
AGameLevelPart::AGameLevelPart()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = false;

    // Initialize other properties as needed
}

// Called when the game starts or when spawned
void AGameLevelPart::BeginPlay()
{
    Super::BeginPlay();

    // Find and populate the array of object spawner components
    FindObjectSpawners();
}

// Spawns objects using the spawner components
void AGameLevelPart::SpawnObjects()
{
    for (UObjectSpawnerComponent* SpawnerComponent : ObjectSpawnerComponents)
    {
        if (SpawnerComponent)
        {
            // Call the SpawnObstacle(), SpawnBooster(), or other relevant methods of the ObjectSpawnerComponent
            SpawnerComponent->SpawnObstacle();
            SpawnerComponent->SpawnBooster();
            // Add more spawning logic based on your specific requirements
        }
    }
}

// Finds and populates the array of object spawner components
void AGameLevelPart::FindObjectSpawners()
{
    TArray<UObjectSpawnerComponent*> FoundComponents;
    GetComponents<UObjectSpawnerComponent>(FoundComponents);

    for (UObjectSpawnerComponent* SpawnerComponent : FoundComponents)
    {
        if (SpawnerComponent)
        {
            ObjectSpawnerComponents.Add(SpawnerComponent);
        }
    }
}