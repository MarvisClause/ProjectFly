// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseFlyPlane.generated.h"

class USpringArmComponent;
class UCameraComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBaseFlyPlane, Log, All);

UCLASS()
class PROJECTFLY_API ABaseFlyPlane : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseFlyPlane();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	TObjectPtr<UStaticMeshComponent> GetStaticMesh() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime);

	// Plane control settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control, meta = (ClampMin = 0.0f))
	float AirControl = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control, meta = (ClampMin = 0.0f))
	float MinimumPlaneSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control, meta = (ClampMin = 0.0f))
	float MaximumPlaneSpeed = 25000.0f;

private:
	// Input process

	void PitchControl(float Value);

	void YawControl(float Value);

	void RollControl(float Value);

	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	// Follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// Static mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMesh;

private:

	// Current flying speed of the plane
	float ForwardSpeed = 0.0f;
};
