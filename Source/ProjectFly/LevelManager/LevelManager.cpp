#include "ProjectFly/LevelManager/LevelManager.h"

ALevelManager::ALevelManager()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = false;
}

void ALevelManager::SpawnNextBiome()
{
    // Ensure Biomes array is not empty
    if (Biomes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Biomes specified in LevelManager."));
        return;
    }

    // Ensure there are enough Biomes to iterate
    if (CurrentBiomeIndex >= Biomes.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("No more Biomes to spawn."));
        return;
    }

    // Get the current biome
    FBiome CurrentBiome = Biomes[CurrentBiomeIndex];

    // Iterate to spawn level parts for the current biome
    for (int32 i = 0; i < LevelPartsPerBiome; ++i)
    {
        // Spawn a random level part from the current biome
        AGameLevelPart* SpawnedPart = SpawnRandomLevelPart(CurrentBiome);

        // If this is not the first level part, connect it to the previous one
        if (GeneratedGameLevelParts.Num() > 0)
        {
            ConnectLevelParts(GeneratedGameLevelParts.Last(), SpawnedPart);
        }

        // Add the spawned level part to the array
        GeneratedGameLevelParts.Add(SpawnedPart);
    }

    // TODO: Temporary commented logic below. We must implement logic for biome transition and connector level parts spawn
    
    // Spawn a connector level part after the generated level parts
    // AConnectorLevelPart* ConnectorPart = SpawnConnectorLevelPart();
    // ConnectLevelParts(GeneratedGameLevelParts.Last(), ConnectorPart);
    // ConnectorLevelParts.Add(ConnectorPart);

    // Move to the next biome for the next spawn
    CurrentBiomeIndex++;
}

AGameLevelPart* ALevelManager::SpawnRandomLevelPart(FBiome Biome)
{
    // TODO: We should rework this part. The only time we use strut in and strut outs are then we need to use connector parts. Other times, we only need
    // straights, turn and elevations. We also need to include logic, which will allow algorithm to check, if it is possible to place a part
    // without colliding with anything else. If you end up hitting something, we should use elevation or turn, which will make it possible to avoid collision.
    // In summary, algorithm should check possibility of level spawn at least in two steps further from the current point.
    
    // Randomly select a level part category from the biome
    ELevelPartCategory RandomCategory = static_cast<ELevelPartCategory>(FMath::RandRange(0, static_cast<int32>(ELevelPartCategory::Elevation)));

    // Generate a random level part based on the selected category
    return Biome.GenerateRandomPart(GetWorld(), RandomCategory);
}

AConnectorLevelPart* ALevelManager::SpawnConnectorLevelPart()
{
    // Implement logic to spawn a connector level part
    // You can customize this function based on your connector level part spawning logic
    return nullptr;
}

void ALevelManager::ConnectLevelParts(ABaseLevelPart* PreviousPart, ABaseLevelPart* NextPart)
{
    // Attach next part to the previous part via connectors
    NextPart->AttachToLevelPart(PreviousPart);
}