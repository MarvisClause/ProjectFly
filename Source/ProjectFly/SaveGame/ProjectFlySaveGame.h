#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ProjectFlySaveGame.generated.h"

UCLASS()
class PROJECTFLY_API UProjectFlySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // Highest unlocked leve
    UPROPERTY(BlueprintReadWrite)
    int32 HighestUnlockedLevel = 0;

    // Current level index
    UPROPERTY(BlueprintReadWrite)
    int32 CurrentLevelIndex = 0;
};