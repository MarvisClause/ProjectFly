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

    // Get random biom index
    CurrentBiomeIndex = FMath::RandRange(0, Biomes.Num() - 1);

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
        AGameLevelPart* SpawnedPart;

        // Spawn strut out at the beginning of the biome
        if (i == 0)
        {
            SpawnedPart = SpawnStrutOutLevelPart(CurrentBiome);
        }
        // Spawn strut in at the end of the biome 
        else if (i == LevelPartsPerBiome - 1)
        {
            SpawnedPart = SpawnStrutInLevelPart(CurrentBiome);
        }
        // Spawn a random level part from the current biome
        else
        {
            SpawnedPart = SpawnRandomLevelPart(CurrentBiome);
        }

        // If this is not the first level part, connect it to the previous one
        if (GeneratedGameLevelParts.Num() > 0)
        {
            ConnectLevelParts(GeneratedGameLevelParts.Last(), SpawnedPart);
        }

        // Spawn objects
        SpawnedPart->SpawnObjects();

        // Add the spawned level part to the array
        GeneratedGameLevelParts.Add(SpawnedPart);
    }

    // Spawn a connector level part after the generated level parts
    AConnectorLevelPart* ConnectorLevelPart = SpawnConnectorLevelPart();
    ConnectLevelParts(GeneratedGameLevelParts.Last(), ConnectorLevelPart);
    GeneratedGameLevelParts.Add(ConnectorLevelPart);

    // Subscribe to the gateway enter event
    ConnectorLevelPart->OnGatewayEnterDelegate.AddDynamic(this, &ALevelManager::OnConnectorLevelPartGatewayEnter);

    // Move to the next biome for the next spawn
    CurrentBiomeIndex++;
}

AGameLevelPart* ALevelManager::SpawnStrutInLevelPart(FBiome Biome)
{
    // Generate strut in
    return Biome.GenerateRandomPart(GetWorld(), ELevelPartCategory::StrutIn);
}

AGameLevelPart* ALevelManager::SpawnStrutOutLevelPart(FBiome Biome)
{
    // Generate strut out
    return Biome.GenerateRandomPart(GetWorld(), ELevelPartCategory::StrutOut);
}

AGameLevelPart* ALevelManager::SpawnRandomLevelPart(FBiome Biome)
{
    // TODO: We should rework this part. The only time we use strut in and strut outs are then we need to use connector parts. Other times, we only need
    // straights, turn and elevations. We also need to include logic, which will allow algorithm to check, if it is possible to place a part
    // without colliding with anything else. If you end up hitting something, we should use elevation or turn, which will make it possible to avoid collision.
    // In summary, algorithm should check possibility of level spawn at least in two steps further from the current point.
    
    // Randomly select a level part category from the biome
    ELevelPartCategory RandomCategory = static_cast<ELevelPartCategory>(FMath::RandRange(static_cast<int32>(ELevelPartCategory::Straight), static_cast<int32>(ELevelPartCategory::Elevation)));

    // Generate a random level part based on the selected category
    return Biome.GenerateRandomPart(GetWorld(), RandomCategory);
}

AConnectorLevelPart* ALevelManager::SpawnConnectorLevelPart()
{
    return GetWorld()->SpawnActor<AConnectorLevelPart>(ConnectorLevelPartClass);
}

void ALevelManager::ConnectLevelParts(ABaseLevelPart* PreviousPart, ABaseLevelPart* NextPart)
{
    // Attach next part to the previous part via connectors
    NextPart->AttachToLevelPart(PreviousPart);
}

void ALevelManager::OnConnectorLevelPartGatewayEnter(AConnectorLevelPart* ConnectorLevelPart)
{
    // Destroy all level parts before connector
    while (GeneratedGameLevelParts[0] != ConnectorLevelPart)
    {
        GeneratedGameLevelParts[0]->Destroy();
        GeneratedGameLevelParts.RemoveAt(0);
    }

    // Spawn new biome
    SpawnNextBiome();
}
