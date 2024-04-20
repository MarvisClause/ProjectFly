#include "ProjectFly/LevelManager/LevelManager.h"
#include "ProjectFly/Pawns/BaseFlyPlane.h"

ALevelManager::ALevelManager()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = false;
}

void ALevelManager::SpawnNextBiome()
{
    // Check, if death wall is not initialized
    if (!IsValid(DeathWall))
    {
        DeathWall = GetWorld()->SpawnActor<ADeathWall>(DeathWallClass);
    }
    else
    {
        DeathWall->ClearAllMovePoints();
    }

    // Ensure Biomes array is not empty
    if (Biomes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Biomes specified in LevelManager."));
        return;
    }

    // Get random biom index
    CurrentBiomeIndex = FMath::RandRange(0, Biomes.Num() - 1);

    // Ensure there are enough Biomes to iterate
    if (CurrentBiomeIndex >= Biomes.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("No more Biomes to spawn."));
        return;
    }

    // Get the current biome
    FBiome CurrentBiome = Biomes[CurrentBiomeIndex];

    // Error edge value, which defines maximum number of times level manager can try to spawn level part
    const int32 ErrorEdgeValue = 10;
    // Error counter
    int32 ErrorCount = 0;
    // Iterate to spawn level parts for the current biome
    for (int32 I = 0; I < LevelPartsPerBiome; ++I)
    {
        // If error count larger than edge value, restart generation of the level
        if (ErrorCount >= ErrorEdgeValue)
        {
            while (GeneratedGameLevelParts.Num() > 0)
            {
                // If we reached connector part, this means, we have cleared up the level
                if (GeneratedGameLevelParts[GeneratedGameLevelParts.Num() - 1]->IsA(AConnectorLevelPart::StaticClass()))
                {
                    break;
                }

                GeneratedGameLevelParts[GeneratedGameLevelParts.Num() - 1]->Destroy();
                GeneratedGameLevelParts.RemoveAt(GeneratedGameLevelParts.Num() - 1);
            }

            ErrorCount = 0;
            I = -1;
            continue;
        }

        AGameLevelPart* SpawnedPart;

        // Spawn strut out at the beginning of the biome
        if (I == 0)
        {
            SpawnedPart = SpawnStrutOutLevelPart(CurrentBiome);
        }
        // Spawn strut in at the end of the biome 
        else if (I == LevelPartsPerBiome - 1)
        {
            SpawnedPart = SpawnStrutInLevelPart(CurrentBiome);
        }
        // Spawn a random level part from the current biome
        else
        {
            SpawnedPart = SpawnRandomLevelPart(CurrentBiome);
        }

        // If this is not the first level part, connect it to the previous one
        if (GeneratedGameLevelParts.Num() > 0)
        {
            ConnectLevelParts(GeneratedGameLevelParts.Last(), SpawnedPart);
        }

        // Perform box cast to check for collisions
        FVector Origin, Extent;
        SpawnedPart->GetActorBounds(false, Origin, Extent);
        FVector BoxLocation = Origin;

        FCollisionQueryParams Params;
        // Ignore current and previous level part, to which current one will be connected
        TArray<AActor*> IgnoredActors;
        IgnoredActors.Add(SpawnedPart);
        if (GeneratedGameLevelParts.Num() > 0)
        {
            IgnoredActors.Add(GeneratedGameLevelParts[GeneratedGameLevelParts.Num() - 1]);
        }
        // Ignore previously spawned part
        Params.AddIgnoredActors(IgnoredActors); 

        FHitResult HitResult;
        bool bHit = GetWorld()->SweepSingleByChannel(HitResult, BoxLocation, BoxLocation, FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeBox(SpawnedPart->GetComponentsBoundingBox(true).GetExtent()), Params);

        // For debug purposes (Currently commented)
        // Draw the box sweep for visualization
        // DrawDebugBox(GetWorld(), BoxLocation, SpawnedPart->GetComponentsBoundingBox(true).GetExtent(), FColor::Green, true, 1000, 0, 5);

        // TODO: Exception for the base fly plane class can be done better. For instance, we could use special collision channel. 
        // If there is collision (Except for base fly plane), abort spawn of the part and repeat the process
        if (bHit && !HitResult.GetActor()->IsA(ABaseFlyPlane::StaticClass()))
        {
            --I;
            ++ErrorCount;
            SpawnedPart->Destroy();
            continue;
        }

        // Spawn objects
        SpawnedPart->SpawnObjects();

        // Add the spawned level part to the array
        GeneratedGameLevelParts.Add(SpawnedPart);

        // Reset error count
        ErrorCount = 0;
    }

    // Spawn a connector level part after the generated level parts
    AConnectorLevelPart* ConnectorLevelPart = SpawnConnectorLevelPart();
    ConnectLevelParts(GeneratedGameLevelParts.Last(), ConnectorLevelPart);
    GeneratedGameLevelParts.Add(ConnectorLevelPart);

    // Add all generated game level parts to the death wall path 
    for (int Index = 0; Index < GeneratedGameLevelParts.Num(); ++Index)
    {
        // If player enters next biome, teleport death wall to the connector
        if (Index == 0)
        {
            DeathWall->SetActorLocation(GeneratedGameLevelParts[Index]->GetFloorStartConnector()->GetComponentLocation());
            DeathWall->SetActorRotation(GeneratedGameLevelParts[Index]->GetFloorStartConnector()->GetComponentRotation());
        }

        FVector Origin, Bounds;
        GeneratedGameLevelParts[Index]->GetActorBounds(false, Origin, Bounds);

        DeathWall->AddMovePoint(GeneratedGameLevelParts[Index]->GetFloorEndConnector()->GetComponentLocation(),
            GeneratedGameLevelParts[Index]->GetFloorEndConnector()->GetComponentRotation(),
            Bounds);
    }

    // Subscribe to the gateway enter event
    ConnectorLevelPart->OnGatewayEnterDelegate.AddDynamic(this, &ALevelManager::OnConnectorLevelPartGatewayEnter);
}

AGameLevelPart* ALevelManager::SpawnStrutInLevelPart(FBiome Biome)
{
    // Generate strut in
    return Biome.GenerateRandomPart(GetWorld(), ELevelPartCategory::StrutIn);
}

AGameLevelPart* ALevelManager::SpawnStrutOutLevelPart(FBiome Biome)
{
    // Generate strut out
    return Biome.GenerateRandomPart(GetWorld(), ELevelPartCategory::StrutOut);
}

AGameLevelPart* ALevelManager::SpawnRandomLevelPart(FBiome Biome)
{
    // TODO: We should rework this part. The only time we use strut in and strut outs are then we need to use connector parts. Other times, we only need
    // straights, turn and elevations. We also need to include logic, which will allow algorithm to check, if it is possible to place a part
    // without colliding with anything else. If you end up hitting something, we should use elevation or turn, which will make it possible to avoid collision.
    // In summary, algorithm should check possibility of level spawn at least in two steps further from the current point.
    
    // Randomly select a level part category from the biome
    ELevelPartCategory RandomCategory = static_cast<ELevelPartCategory>(FMath::RandRange(static_cast<int32>(ELevelPartCategory::Straight), static_cast<int32>(ELevelPartCategory::Elevation)));

    // Generate a random level part based on the selected category
    return Biome.GenerateRandomPart(GetWorld(), RandomCategory);
}

AConnectorLevelPart* ALevelManager::SpawnConnectorLevelPart()
{
    return GetWorld()->SpawnActor<AConnectorLevelPart>(ConnectorLevelPartClass);
}

void ALevelManager::ConnectLevelParts(ABaseLevelPart* PreviousPart, ABaseLevelPart* NextPart)
{
    // Attach next part to the previous part via connectors
    NextPart->AttachToLevelPart(PreviousPart);
}

void ALevelManager::OnConnectorLevelPartGatewayEnter(AConnectorLevelPart* ConnectorLevelPart)
{
    // Destroy all level parts before connector
    while (GeneratedGameLevelParts[0] != ConnectorLevelPart)
    {
        GeneratedGameLevelParts[0]->Destroy();
        GeneratedGameLevelParts.RemoveAt(0);
    }

    // Spawn new biome
    SpawnNextBiome();
}
