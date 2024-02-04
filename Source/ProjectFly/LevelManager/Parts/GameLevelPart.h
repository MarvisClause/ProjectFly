#pragma once

#include "ProjectFly/LevelManager/Parts/BaseLevelPart.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectFly/LevelManager/Components/ObjectSpawnerComponent.h" 
#include "GameLevelPart.generated.h"

UCLASS(Abstract)
class PROJECTFLY_API AGameLevelPart : public ABaseLevelPart
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AGameLevelPart();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Spawns objects using the spawner components
    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void SpawnObjects();

protected:
    // Array of object spawner components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
    TArray<UObjectSpawnerComponent*> ObjectSpawnerComponents;

private:
    // Finds and populates the array of object spawner components
    void FindObjectSpawners();
};