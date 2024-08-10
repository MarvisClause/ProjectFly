#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectSpawnerComponent.generated.h"

USTRUCT(BlueprintType)
struct FSpawnObjectData
{
    GENERATED_BODY()

    // Defines if the object needs to be snapped to the floor on spawn
    UPROPERTY(EditAnywhere, Category = "SpawnData")
    bool bSnapToFloorOnSpawn = false;

    // Defines if the object needs to be rotated parallel to the floor
    UPROPERTY(EditAnywhere, Category = "SpawnData")
    bool bRotateParallelToFloor = false;

    // Spawn chance of object
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SpawnObjects, meta = (ClampMin = 0.0f, ClampMax = 100.0f))
    float SpawnChance = 10.0f;

    // Add object spawn data here
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
    TArray<FSpawnObjectData> ObstaclesToSpawn;

    // Array of booster spawn data
    UPROPERTY(EditAnywhere, Category = "SpawnData")
    TArray<FSpawnObjectData> BoostersToSpawn;

    // Pointer to the currently spawned object
    UPROPERTY(BlueprintReadOnly, Category = "SpawnedObject")
    AActor* SpawnedObject = nullptr;

    // Destroys the currently spawned object
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void DestroySpawnedObject();

private:
    // Spawns an object based on spawn data
    void SpawnObject(const FSpawnObjectData&  SpawnObject);
};