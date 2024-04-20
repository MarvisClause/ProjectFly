#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectFly/LevelManager/Biome.h"
#include "ProjectFly/LevelManager/Parts/GameLevelPart.h"
#include "ProjectFly/LevelManager/Parts/ConnectorLevelPart.h"
#include "ProjectFly/Objects/DeathWall.h"
#include "LevelManager.generated.h"

UCLASS(Abstract)
class PROJECTFLY_API ALevelManager : public AActor
{
    GENERATED_BODY()

public:
    ALevelManager();

    // Function to spawn the next biome in the level sequence
    UFUNCTION(BlueprintCallable, Category = "LevelManager")
    void SpawnNextBiome();

protected:
    // Array of Biome objects
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelManager")
    TArray<FBiome> Biomes;

    // Connector level part class
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelManager")
    TSubclassOf<AConnectorLevelPart> ConnectorLevelPartClass;

    // Death wall class
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelManager")
    TSubclassOf<ADeathWall> DeathWallClass;

    // TODO: This number should be a somewhat random one, which will allows us to scale level generation size
    // Number of level parts to generate per biome
    UPROPERTY(EditDefaultsOnly, Category = "LevelManager")
    int32 LevelPartsPerBiome;

private:
    // Spawns strut in level part of the given biome. Should be called before connector.
    AGameLevelPart* SpawnStrutInLevelPart(FBiome Biome);

    // Spawns strut out level part of the given biome. Should be called after connector.
    AGameLevelPart* SpawnStrutOutLevelPart(FBiome Biome);

    // Helper function to spawn a random level part from the given biome
    AGameLevelPart* SpawnRandomLevelPart(FBiome Biome);

    // Helper function to spawn a connector level part
    AConnectorLevelPart* SpawnConnectorLevelPart();

    // Helper function to connect two level parts using connectors
    void ConnectLevelParts(ABaseLevelPart* PreviousPart, ABaseLevelPart* NextPart);

    // Method for handling gateway enter of connector part event
    UFUNCTION()
    void OnConnectorLevelPartGatewayEnter(AConnectorLevelPart* ConnectorLevelPart);

    // Current index to track the next biome to spawn
    int32 CurrentBiomeIndex;

    // Array of AGameLevelPart objects
    UPROPERTY()
    TArray<ABaseLevelPart*> GeneratedGameLevelParts;

    // Death wall object
    UPROPERTY()
    ADeathWall* DeathWall = nullptr; 
};
