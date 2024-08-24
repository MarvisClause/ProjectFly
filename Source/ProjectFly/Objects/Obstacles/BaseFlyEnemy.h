#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseFlyEnemy.generated.h"

class USphereComponent;
class ABaseFlyPlane;

// Base enemy class
// By default acts like a projectile, which shoots itself towards the plane
UCLASS()
class PROJECTFLY_API ABaseFlyEnemy : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABaseFlyEnemy();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called when something enters the trigger area
    UFUNCTION()
    void OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Called when something leaves the trigger area
    UFUNCTION()
    void OnTriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex);

                                    // Defines the delay before the enemy locks onto a target
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float DelayBeforeTargetLock = 5.0f;

    // Defines the enemy speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float MovementSpeed = 1500.0f;

    // Defines the enemy acceleration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float MovementAcceleration = 3000.0f;

    // Rotation speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float RotationSpeed = 5.0f;

    // Static mesh component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    // Trigger area component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> TargetLockTriggerArea;

private:
    // The locked target plane
    TObjectPtr<ABaseFlyPlane> LockedTarget;

    // Timer for target locking
    float TargetLockTime = 0.0f;

    // Indicates if the target has been locked
    bool IsTargetLocked = false;

    // List of affected planes
    TArray<TObjectPtr<ABaseFlyPlane>> AffectedPlanesArray;
};
