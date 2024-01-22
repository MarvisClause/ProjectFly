#pragma once

#include "CoreMinimal.h"
#include "ProjectFly/LevelManager/Parts/GameLevelPart.h" // Assuming there's a base class for game level parts
#include "Biome.generated.h"

UENUM(BlueprintType)
enum class ELevelPartCategory : uint8
{
	StrutIn,
	StrutOut,
	Straight,
	Turn,
	Elevation
};

UCLASS()
class PROJECTFLY_API UBiome : public UObject
{
	GENERATED_BODY()

public:
	// Function to generate a random sequence of level parts based on the biome
	UFUNCTION(BlueprintCallable, Category = "Biome")
	TArray<AGameLevelPart*> GenerateRandomLevelSequence();

	// Function to generate a random level part for a specific category
	AGameLevelPart* GenerateRandomPart(ELevelPartCategory Category);

protected:
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