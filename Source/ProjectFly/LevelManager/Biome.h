#pragma once

#include "CoreMinimal.h"
#include "ProjectFly/LevelManager/Parts/GameLevelPart.h" // Assuming there's a base class for game level parts
#include "Biome.generated.h"

UENUM(BlueprintType)
enum class ELevelPartCategory : uint8
{
	// Special parts 
	StrutIn,
	StrutOut,
	// Regular parts
	Straight,
	Turn,
	Elevation
};

USTRUCT(BlueprintType)
struct PROJECTFLY_API FBiome
{
	GENERATED_BODY()

public:
	
	// Function to generate a random level part for a specific category
	AGameLevelPart* GenerateRandomPart(UWorld* World, ELevelPartCategory Category);

	// Array of AGameLevelPart subclasses for "strut" pieces coming into this biome
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome")
	TArray<TSubclassOf<AGameLevelPart>> StrutIns;

	// Array of AGameLevelPart subclasses for "strut" pieces leaving this biome
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome")
	TArray<TSubclassOf<AGameLevelPart>> StrutOuts;

	// Array of straight hallway pieces for this biome
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome")
	TArray<TSubclassOf<AGameLevelPart>> Straights;

	// Array of turning hallway pieces for this biome
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome")
	TArray<TSubclassOf<AGameLevelPart>> Turns;

	// Array of elevation change pieces (ramps, stairs, etc) for this biome
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome")
	TArray<TSubclassOf<AGameLevelPart>> Elevations;
};