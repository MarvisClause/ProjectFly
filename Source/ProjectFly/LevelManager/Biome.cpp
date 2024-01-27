#include "ProjectFly/LevelManager/Biome.h"



AGameLevelPart* UBiome::GenerateRandomPart(ELevelPartCategory Category)
{
    TArray<TSubclassOf<AGameLevelPart>>* SelectedArray = nullptr;

    // Select the appropriate array based on the category
    switch (Category)
    {
    case ELevelPartCategory::StrutIn:
        SelectedArray = &StrutIns;
        break;
    case ELevelPartCategory::StrutOut:
        SelectedArray = &StrutOuts;
        break;
    case ELevelPartCategory::Straight:
        SelectedArray = &Straights;
        break;
    case ELevelPartCategory::Turn:
        SelectedArray = &Turns;
        break;
    case ELevelPartCategory::Elevation:
        SelectedArray = &Elevations;
        break;
    default:
        // Handle default case or provide a fallback
        break;
    }

    // Check if the selected array is valid
    if (SelectedArray && SelectedArray->Num() > 0)
    {
        // Generate a random index within the array
        int32 RandomIndex = FMath::RandRange(0, SelectedArray->Num() - 1);

        // Spawn the selected game level part
        AGameLevelPart* SpawnedPart = GetWorld()->SpawnActor<AGameLevelPart>((*SelectedArray)[RandomIndex]);

        return SpawnedPart;
    }

    return nullptr;
}