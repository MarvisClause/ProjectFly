#include "ObjectSpawnerComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UObjectSpawnerComponent::UObjectSpawnerComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UObjectSpawnerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UObjectSpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    DestroySpawnedObject();
}

// Spawns an obstacle with probability-based selection
void UObjectSpawnerComponent::SpawnObstacle()
{
    // Implement spawning logic for obstacles with probability
    if (ObstaclesToSpawn.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, ObstaclesToSpawn.Num() - 1);
        SpawnObject(ObstaclesToSpawn[RandomIndex]);
    }
}

// Spawns a booster with probability-based selection
void UObjectSpawnerComponent::SpawnBooster()
{
    // Implement spawning logic for boosters with probability
    if (BoostersToSpawn.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, BoostersToSpawn.Num() - 1);
        SpawnObject(BoostersToSpawn[RandomIndex]);
    }
}

// Destroys the currently spawned object
void UObjectSpawnerComponent::DestroySpawnedObject()
{
    if (IsValid(SpawnedObject))
    {
        SpawnedObject->Destroy();
        SpawnedObject = nullptr;
    }
}

// Spawns an object based on spawn data
void UObjectSpawnerComponent::SpawnObject(const FSpawnObjectData& SpawnObject)
{
    if (!IsValid(SpawnObject.ObjectClass))
    { 
        ensureMsgf(IsValid(SpawnObject.ObjectClass), TEXT("Object class was not defined for object spawner data!"));
        return;
    }

    FVector SpawnPosition = GetComponentLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;

    // Check, if object should be altered in some way
    if (SpawnObject.bSnapToFloorOnSpawn || SpawnObject.bRotateParallelToFloor)
    {
        FVector End = SpawnPosition - FVector(0.0f, 0.0f, 10000.0f);

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(SpawnedObject);

        if (GetWorld()->LineTraceSingleByChannel(HitResult, SpawnPosition, End, ECC_Visibility, Params))
        {
            // Draw the line for debugging
            DrawDebugLine(GetWorld(), SpawnPosition, End, FColor::Green, false, 1, 0, 1);

            // Snap to the hit location
            if (SpawnObject.bSnapToFloorOnSpawn)
            {
                SpawnPosition = HitResult.Location;
            }

            // Calculate the rotation to align parallel to the hit surface
            if (SpawnObject.bRotateParallelToFloor)
            {
                FVector SurfaceNormal = HitResult.ImpactNormal;
                FVector ForwardVector = FVector::CrossProduct(SurfaceNormal, FVector::UpVector).GetSafeNormal();
                FVector RightVector = FVector::CrossProduct(SurfaceNormal, ForwardVector).GetSafeNormal();
                SpawnRotation = FRotationMatrix::MakeFromXZ(ForwardVector, SurfaceNormal).Rotator();
            }
        }
        else
        {
            // Draw the line for debugging
            DrawDebugLine(GetWorld(), SpawnPosition, End, FColor::Red, false, 1, 0, 1);
        }
    }

    // Implement spawning logic using SpawnObject
    if (FMath::RandRange(1, 100) <= SpawnObject.SpawnChance)
    {
        SpawnedObject = GetWorld()->SpawnActor<AActor>(SpawnObject.ObjectClass, SpawnPosition, SpawnRotation);
    }
}