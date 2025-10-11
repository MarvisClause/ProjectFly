// Property of (U.G.O.F, Stranget Badger, Chet kak to tak, Friends) 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoosterObject.generated.h"

class UBoxComponent;
class AGliderPawn;

// Pushes plane, if it enters booster trigger area
UCLASS(Abstract)
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
	
	// Defines, if bost applied to the plane will decrease the further the plane is from the source or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Booster")
	bool bDistanceBasedBoosterPush = true;

	// Defines, if boost applied to the plane will be applied slowly and reach it's max power or instantly
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Booster")
	bool bIncrementalEnterBoosterPush = true;

	// Defines, if boost applied to the plane will be reduced slowly and reach it's zero power or instantly
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Booster")
	bool bIncrementalExitBoosterPush = true;

	// Booster speed increase value
	// Increases speed of the plane
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Booster", meta = (ClampMin = 0.0f))
	float BoosterSpeedIncreaseValue = 5.0f;

	// Booster push scalar
	// Pushes plane
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Booster", meta = (ClampMin = 0.0f))
	float BoosterPushScalar = 500.0f;

	// Booster rotation scalar
	// Rotates plane
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Booster", meta = (ClampMin = 0.0f))
	float BoosterRotationScalar = 2.0f;

	// Defines how long after exit the boost fades
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Booster")
	float BoosterFalloffDuration = 1.0f;

	// Scene component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> RootSceneComponent;

	// Static mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	// Booster trigger area
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoosterTriggerArea;

private:
	float CalculateDistanceFactor(AGliderPawn* Glider) const;

	float ComputeFinalInfluence(float BaseInfluence, float DistanceFactor) const;

	void ApplyBoosterEffect(AGliderPawn* Glider, float Influence, float DeltaTime, bool bAlignToBooster);

	void HandleActiveGliders(float DeltaTime);

	void HandleExitingGliders(float DeltaTime);

	// Affects plane, which enters trigger area
	// Holds information about plane and booster object influence on it
	TArray<TTuple<TObjectPtr<AGliderPawn>, float>> AffectedGlidersArray;

	// Planes, which are leaving booster area
	TArray<TPair<TObjectPtr<AGliderPawn>, float>> ExitingGlidersArray;
};
