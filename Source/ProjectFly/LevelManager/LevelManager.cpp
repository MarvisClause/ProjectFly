#include "ProjectFly/LevelManager/LevelManager.h"

void ULevelManager::SpawnNextBiome()
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
    UBiome* CurrentBiome = Biomes[CurrentBiomeIndex];

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

    // Spawn a connector level part after the generated level parts
    AConnectorLevelPart* ConnectorPart = SpawnConnectorLevelPart();
    ConnectLevelParts(GeneratedGameLevelParts.Last(), ConnectorPart);
    ConnectorLevelParts.Add(ConnectorPart);

    // Move to the next biome for the next spawn
    CurrentBiomeIndex++;
}

AGameLevelPart* ULevelManager::SpawnRandomLevelPart(UBiome* Biome)
{
    // Randomly select a level part category from the biome
    ELevelPartCategory RandomCategory = static_cast<ELevelPartCategory>(FMath::RandRange(0, static_cast<int32>(ELevelPartCategory::Elevation)));

    // Generate a random level part based on the selected category
    return Biome->GenerateRandomPart(RandomCategory);
}

AConnectorLevelPart* ULevelManager::SpawnConnectorLevelPart()
{
    // Implement logic to spawn a connector level part
    // You can customize this function based on your connector level part spawning logic
    return nullptr;
}

void ULevelManager::ConnectLevelParts(ABaseLevelPart* PreviousPart, ABaseLevelPart* NextPart)
{
    // Implement logic to connect two level parts using connectors
    // Customize this function based on your connector logic
}