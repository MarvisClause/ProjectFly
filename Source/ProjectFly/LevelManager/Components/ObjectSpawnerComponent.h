#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectSpawnerComponent.generated.h"

USTRUCT(BlueprintType)
struct FSpawnObstacleData
{
    GENERATED_BODY()

    // Add obstacle spawn data here
    UPROPERTY(EditAnywhere, Category = "SpawnData")
    TSubclassOf<AActor> ObjectClass;
};

USTRUCT(BlueprintType)
struct FSpawnBoosterData
{
    GENERATED_BODY()

    // Add booster spawn data here
    UPROPERTY(EditAnywhere, Category = "SpawnData")
    TSubclassOf<AActor> ObjectClass;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTFLY_API UObjectSpawnerComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UObjectSpawnerComponent();

    // Called when the game starts
    virtual void BeginPlay() override;

    // Called when the object is destroyed
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Spawns an obstacle with probability-based selection
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void SpawnObstacle();

    // Spawns a booster with probability-based selection
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void SpawnBooster();

protected:
    // Array of obstacle spawn data
    UPROPERTY(EditAnywhere, Category = "SpawnData")
    TArray<FSpawnObstacleData> ObstaclesToSpawn;

    // Array of booster spawn data
    UPROPERTY(EditAnywhere, Category = "SpawnData")
    TArray<FSpawnBoosterData> BoostersToSpawn;

    // Pointer to the currently spawned object
    UPROPERTY(BlueprintReadOnly, Category = "SpawnedObject")
    AActor* SpawnedObject;

    // Destroys the currently spawned object
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void DestroySpawnedObject();

private:
    // Spawns an object based on spawn data
    void SpawnObject(const TSubclassOf<AActor> SpawnObject);
};