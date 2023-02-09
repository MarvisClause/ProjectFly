// Property of (U.G.O.F, Stranget Badger, Chet kak to tak, Friends) 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoosterObject.generated.h"

class UBoxComponent;
class ABaseFlyPlane;

// Pushes plane, if it enters booster trigger area
UCLASS()
class PROJECTFLY_API ABoosterObject : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABoosterObject();

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

	// Booster power scalar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control, meta = (ClampMin = 0.0f))
	float BoosterPowerScalar = 2500.0f;

	// Static mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	// Booster trigger area
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoosterTriggerArea;

private:
	// Affects plane, which enters trigger area
	TArray<TObjectPtr<ABaseFlyPlane>> AffectedPlanesArray;
};
