// Property of (U.G.O.F, Stranget Badger, Chet kak to tak, Friends) 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeathWall.generated.h"

class UBoxComponent;
class ABaseFlyPlane;

// Chases plane by always moving towards its location and decreasing its speed once collided
UCLASS()
class PROJECTFLY_API ADeathWall : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADeathWall();

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
	float ChaseSpeedValue = 1.0f;

	// Scene component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BaseSceneComponent;

	// Trigger area
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> SpeedDecreaseTriggerArea;

private:
	// Target plane, which will be followed by death wall
	TObjectPtr<ABaseFlyPlane> TargetPlane;

	// Affects plane, which enters trigger area
	// Holds information about plane and booster object influence on it
	TArray<TTuple<TObjectPtr<ABaseFlyPlane>, float>> AffectedPlanesArray;
};
