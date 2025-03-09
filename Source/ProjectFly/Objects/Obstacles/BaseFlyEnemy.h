#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseFlyEnemy.generated.h"

class USphereComponent;
class ABaseFlyPlane;

enum class EEnemyState
{
    Follow,
    Aim,
    Charge
};

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
    float DelayBeforeTargetLock = 1.0f;

    // Aim time
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float AimTime = 2.0f;

    // Charge time
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float ChargeTime = 5.0f;

    // Defines the enemy speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float MovementSpeed = 10000.0f;

    // Defines the enemy acceleration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float MovementAcceleration = 30000.0f;

    // Rotation speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float RotationSpeed = 3.0f;

    // Defines charge radius
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float ChargeTriggerRadius = 10000.0f;

    // Defines the enemy rotation during charge
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float ChargeRotationSpeed = 20.0f;

    // Defines the enemy acceleration during charge
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float ChargeAcceleration = 30000.0f;

    // Define the enemy charge yaw rotation factor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float ChargeYawRotationFactor = 150.0f;

    // Static mesh component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    // Trigger area component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> TargetLockTriggerArea;

private:
    void FollowTarget(const FVector& DirectionToTarget, float DeltaTime);

    void AimAtTarget(const FVector& DirectionToTarget, float DeltaTime);

    void ChargeTowardsPosition(const FVector& DirectionToTarget, float DeltaTime);

    // The locked target plane
    TObjectPtr<ABaseFlyPlane> LockedTarget;

    // Timer for target locking
    float TargetLockTime = 0.0f;

    // Indicates if the target has been locked
    bool IsTargetLocked = false;

    // Defines current state 
    EEnemyState CurrentState = EEnemyState::Follow;
    
    // Timer for aim
    float AimStartTime = 0.0f;

    // Timer for charge
    float ChargeStartTime = 0.0f;

    // Calculated charge position
    FVector CalculatedChargePosition;

    // List of affected planes
    TArray<TObjectPtr<ABaseFlyPlane>> TargetPlanesArray;
};
