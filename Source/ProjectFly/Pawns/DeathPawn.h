#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DeathPawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class PROJECTFLY_API ADeathPawn : public APawn
{
	GENERATED_BODY()

public:
	ADeathPawn();

	// Inherit velocity from original pawn
	void ApplyInheritedVelocity(const FVector& Velocity);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	///////////////////////// Components
	
	// Destroyed glider mesh (physics enabled)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Death Pawn - Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WreckMesh;

	// Optional camera for player death view
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Death Pawn - Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Death Pawn - Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	///////////////////////// Control varaibles

	// Torque applied when spawned
	UPROPERTY(EditAnywhere, Category = "Death Pawn - Physics")
	FVector InitialTorque = FVector(500.f, 250.f, 800.f);

	// How long before the wreck disappears automatically
	UPROPERTY(EditAnywhere, Category="Death Pawn - Auto Destroy")
	float AutoDestroyDelay = 10.f;

	// Whether player can rotate camera after death
	UPROPERTY(EditAnywhere, Category="Death Pawn - Camera")
	bool bAllowCameraRotation = true;

	UPROPERTY(EditAnywhere, Category = "Death Pawn - Camera", meta = (ClampMin = 0.0f))
	float MouseSensitivity = 1.0f;

	///////////////////////// Input variables

	float DirectionCameraYaw;
	float DirectionCameraPitch;

	///////////////////////// Methods

	// Let player rotate camera (no movement)
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Camera input
	void LookUp(float Value);
	void Turn(float Value);
};
