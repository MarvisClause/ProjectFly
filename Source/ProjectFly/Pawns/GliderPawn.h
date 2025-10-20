#pragma once

#include "ProjectFly/Interface/FlightMouseAimInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GliderPawn.generated.h"

UCLASS()
class PROJECTFLY_API AGliderPawn : public APawn, public IFlightMouseAimInterface
{
	GENERATED_BODY()

public:
	AGliderPawn();
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual FVector GetTargetAimWorldLocation() const override;
	virtual FVector GetCurrentDirection() const override;
	virtual FRotator GetCurrentRotation() const override;
	virtual void SetDesiredDirection(FVector WorldDirection) override;

	virtual void Tick(float DeltaTime) override;

	void AffectSpeed(float Speed);

	void StartRemovingCameraLag();
	void StartEnablingCameraLag();

	UStaticMeshComponent* GetStaticMesh() const;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Calculates change of speed from inclination 
	void CalculateSpeed(float DeltaTime);

	UFUNCTION()
	void OnGliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	// Input variables
	float CameraYaw;
	float CameraPitch;

	FVector DesiredDirection;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Turn Control", meta = (ClampMin = 0.0f))
	FVector TurnTorque = FVector(45.f, 25.f, 45.f);

	UPROPERTY(EditAnywhere, Category = "Glider Control - Turn Control", meta = (ClampMin = 0.0f))
	float MouseSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Turn Control", meta = (ClampMin = 0.0f))
	float TurnAngleSensitivity = 1.8f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Turn Control", meta = (ClampMin = 0.0f))
	float AggressiveTurnAngle = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Lift Control", meta = (ClampMin = 0.0f))
	float LiftCoefficientScalar = 0.003f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Lift Control", meta = (ClampMin = 0.0f))
	float MaxLiftForce = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Speed Control")
	float MinimumPlaneSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Speed Control", meta = (ClampMin = 0.0f))
	float MaximumPlaneSpeed = 160000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Speed Control", meta = (ClampMin = 0.0f))
	float StartPlaneSpeed = 80000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Speed Control", meta = (ClampMin = 0.0f))
	float DiveSpeedIncreaseScalar = 30000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Speed Control", meta = (ClampMin = 0.0f))
	float RiseSpeedDecreaseScalar = 32000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Air Control", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float MinimumAirControl = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Air Control", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float MaximumAirControl = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Gravity Control", meta = (ClampMin = 0.0f))
	float GravityScalar = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Gravity Control", meta = (ClampMin = 0.0f))
	float GravityMultiplier = 1.0f; 

	// Forward speed of the plane
	// This is the main variable, which defines speed of the plane 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ForwardSpeed = 0.0f;

	// Dash Settings
	FTimerHandle DashStopTimer;
	FTimerHandle DashCooldownTimer;

	bool bCanDash = true;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float DashSpeedCost = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float DashStrength = 1600000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float DashCooldown = 3.0f;

	// Halt Settings
	FTimerHandle HaltTimer;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Halt")
	float HaltSpeedCost = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Halt")
	float HaltDuration = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Halt")
	float HaltCooldown = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Halt")
	float HaltSpeedLinearDamping = 2.0f;

	float LinearDampingBeforeHaltBackup = 0.0f;

	bool bCanHalt = true;
	bool bIsHalting = false;

	// Camera Settings

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera")
	float NormalLagSpeed = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera")
	float MaxLagSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera")
	float CameraLagTransitionSpeedToNormal = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera")
	float CameraLagTransitionSpeedToMax = 0.05f;

	// Camera Lag Transition State
	enum class ECameraLagTransitionState
	{
		None,
		Increasing,
		Decreasing
	};
	ECameraLagTransitionState CameraLagState = ECameraLagTransitionState::None;

	// Turbulence
	UPROPERTY(EditAnywhere, Category = "Glider Control - Turbulence")
	float TurbulenceScalar = 3.0f;

	// Key responsiveness
	UPROPERTY(EditAnywhere, Category = "Glider Control - Key Control")
	float KeyResponsivenessScalar = 10.0f;

	// Boolean variable, which controls autopilot work
	bool bEnableAutopilot = true;
	UPROPERTY(EditAnywhere, Category = "Glider Control - Manual Control Timeout")
	float ManualControlTimeout = 2.0f;
	FTimerHandle AutopilotEnableTimer;

	// Autopilot controls
	void DisableAutopilotTemporarily();
	void EnableAutopilot();

	// Camera
	void UpdateCameraLagTransition(float DeltaTime);

	// Dash
	void StartDash();
	void ResetDashCooldown();

	// Halt
	void StartHalt();
	void StopHalt();
	void ResetHaltCooldown();

	// Mouse input handlers
	void LookUp(float Value);
	void Turn(float Value);

	// Keys input handlers
	void MovePitch(float Value);
	void MoveYaw(float Value);
	void MoveRoll(float Value);

	void RunAutopilot(const FVector& FlyTarget, float& OutYaw, float& OutPitch, float& OutRoll);
};
