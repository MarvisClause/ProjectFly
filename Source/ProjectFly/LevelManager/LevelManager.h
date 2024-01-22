#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectFly/LevelManager/Biome.h"
#include "ProjectFly/LevelManager/Parts/GameLevelPart.h"
#include "ProjectFly/LevelManager/Parts/ConnectorLevelPart.h"
#include "LevelManager.generated.h"

UCLASS()
class PROJECTFLY_API ULevelManager : public UObject
{
    GENERATED_BODY()

public:
    // Function to spawn the next biome in the level sequence
    UFUNCTION(BlueprintCallable, Category = "LevelManager")
    void SpawnNextBiome();

protected:
    // Array of Biome objects
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelManager")
    TArray<UBiome*> Biomes;

    // Number of level parts to generate per biome
    UPROPERTY(EditDefaultsOnly, Category = "LevelManager")
    int32 LevelPartsPerBiome;

private:
    // Helper function to spawn a random level part from the given biome
    AGameLevelPart* SpawnRandomLevelPart(UBiome* Biome);

    // Helper function to spawn a connector level part
    AConnectorLevelPart* SpawnConnectorLevelPart();

    // Helper function to connect two level parts using connectors
    void ConnectLevelParts(ABaseLevelPart* PreviousPart, ABaseLevelPart* NextPart);

    // Current index to track the next biome to spawn
    int32 CurrentBiomeIndex;

    // Array of AGameLevelPart objects
    UPROPERTY()
    TArray<AGameLevelPart*> GeneratedGameLevelParts;

    // Array of AConnectorLevelPart objects
    UPROPERTY()
    TArray<AConnectorLevelPart*> ConnectorLevelParts;
};
