// Property of (U.G.O.F, Stranget Badger, Chet kak to tak, Friends) 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeathWall.generated.h"

class UBoxComponent;
class ABaseFlyPlane;

struct DeathWallMovePoint
{
	FVector TargetPosition;
	FRotator TargetRotation;
	FVector NewDeathWallBoxExtent;
	// Defines, if death wall was resized to fit to the move point
	bool bWasResized = false;
};

// Chases plane by always moving towards its location and decreasing its speed once collided
UCLASS(Abstract)
class PROJECTFLY_API ADeathWall : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADeathWall();

	// Set move point
	void SetMoveSpeed(float MoveSpeed);

	// Sets move target for the death wall
	void AddMovePoint(FVector TargetPosition, FRotator TargetRotation, FVector NewDeathWallBoxExtent);

	// Clears all move points
	void ClearAllMovePoints();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime);

	UFUNCTION()
	virtual void OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex);

	// Defines, how speed is decreased per tick once plane is inside the collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control, meta = (ClampMin = 0.0f))
	float SpeedDecreaseValue = 5.0f;

	// Defines, death wall rotation speed value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control, meta = (ClampMin = 0.0f))
	float RotationSpeedValue = 0.1f;

	// Defines, death wall chase speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control, meta = (ClampMin = 0.0f))
	float MoveSpeedValue = 1.0f;

	// Scene component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BaseSceneComponent;

	// Trigger area
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> SpeedDecreaseTriggerArea;

private:
	// Affects plane, which enters trigger area
	// Holds information about plane and booster object influence on it
	TArray<TTuple<TObjectPtr<ABaseFlyPlane>, float>> AffectedPlanesArray;

	// Move points, where death wall should be
	TArray<DeathWallMovePoint> DeathWallMovePointArray;
};
