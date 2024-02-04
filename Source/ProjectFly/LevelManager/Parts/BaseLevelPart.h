#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseLevelPart.generated.h"

UCLASS(Abstract)
class PROJECTFLY_API ABaseLevelPart : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABaseLevelPart();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Returns the start connector component
    USceneComponent* GetFloorStartConnector() const;

    // Returns the end connector component
    USceneComponent* GetFloorEndConnector() const; 

    // Attaches this level part to another ABaseLevelPart via their connector components
    void AttachToLevelPart(ABaseLevelPart* OtherLevelPart);

protected:
    // Sets location by start connect
    void SetLocationByStartConnector(const FVector& Location);

    // Floor start connector component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LevelPart")
    USceneComponent* FloorStartConnector;

    // Floor end connector component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LevelPart")
    USceneComponent* FloorEndConnector;

    // Visible floor mesh component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LevelPart")
    UStaticMeshComponent* FloorMesh;  

    // Visible base scene component mesh component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LevelPart")
    USceneComponent* BaseFloorSceneComponent;
};