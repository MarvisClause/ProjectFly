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

	UPROPERTY(EditAnywhere, Category = "Glider Control - Stall Control")
	float StallPlaneSpeedThreshold = 30000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Stall Control")
	float StallRotationForce = 15.0f;

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
	float MinimumAirControl = 0.3f;

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

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash", meta = (ClampMin = 0.0f))
	float DashSpeedCost = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash", meta = (ClampMin = 0.0f))
	float DashStrength = 1600000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash", meta = (ClampMin = 0.1f))
	float DashCooldown = 3.0f;

	// Halt Settings
	bool bHaltInputActive = false;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Halt", meta = (ClampMin = 0.0f))
	float HaltSpeedReduction = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Halt", meta = (ClampMin = 0.0f))
	float HaltSpeedLinearDamping = 4.0f;

	float LinearDampingBeforeHaltBackup = 0.0f;

	// Camera Settings

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
	float NormalLagSpeed = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
	float MaxLagSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
	float CameraLagTransitionSpeedToNormal = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
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
	UPROPERTY(EditAnywhere, Category = "Glider Control - Turbulence", meta = (ClampMin = 0.0f))
	float TurbulenceScalar = 3.0f;

	// Key responsiveness
	UPROPERTY(EditAnywhere, Category = "Glider Control - Key Control Response", meta = (ClampMin = 0.0f))
	float PitchMinKeyResponsivenessScalar = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Glider Control - Key Control Response", meta = (ClampMin = 0.0f))
	float PitchMaxKeyResponsivenessScalar = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Key Control Response", meta = (ClampMin = 0.0f))
	float YawMinKeyResponsivenessScalar = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Glider Control - Key Control Response", meta = (ClampMin = 0.0f))
	float YawMaxKeyResponsivenessScalar = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Key Control Response", meta = (ClampMin = 0.0f))
	float RollMinKeyResponsivenessScalar = 15.0f;
	UPROPERTY(EditAnywhere, Category = "Glider Control - Key Control Response", meta = (ClampMin = 0.0f))
	float RollMaxKeyResponsivenessScalar = 20.0f;

	// Timer for roll logic control
	UPROPERTY(EditAnywhere, Category = "Glider Control - Aggressive Turn Angle Disable Timeout", meta = (ClampMin = 0.1f))
	float AggressiveTurnAngleDisableTimeout = 0.5f;
	FTimerHandle EnableAggressiveTurnAngleTimer;

	// Boolean variable, which controls autopilot pitch and roll
	bool bDisableAutopilot = false;
	UPROPERTY(EditAnywhere, Category = "Glider Control - Manual Control Timeout", meta = (ClampMin = 0.1f))
	float DisableAutopilotTimeout = 0.1f;
	FTimerHandle DisableAutopilotEnableTimer;

	// Impact threshold for configuring plane 
	UPROPERTY(EditAnywhere, Category = "Glider Control - Impact Control", meta = (ClampMin = 0.0f))
	float MinorImpactThreshold = 300.0f;
	UPROPERTY(EditAnywhere, Category = "Glider Control - Impact Control", meta = (ClampMin = 0.0f))
	float MajorImpactThreshold = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Impact Control", meta = (ClampMin = 0.0f), meta = (ClampMax = 1.0f))
	float MinorImpactPercent = 0.3f;
	UPROPERTY(EditAnywhere, Category = "Glider Control - Impact Control", meta = (ClampMin = 0.0f), meta = (ClampMax = 1.0f))
	float MajorImpactPercent = 0.8f;

	// Autopilot state control
	void DisableAutopilotTemporarily();

	// Roll controls
	void DisableAggressiveTurnAngleTemporarily();

	// Camera
	void UpdateCameraLagTransition(float DeltaTime);

	// Dash
	void StartDash();
	void ResetDashCooldown();

	// Halt
	void StartHalt();
	void StopHalt();

	// Mouse input handlers
	void LookUp(float Value);
	void Turn(float Value);

	// Keys input handlers
	void MovePitch(float Value);
	void MoveYaw(float Value);
	void MoveRoll(float Value);

	void RunAutopilot(const FVector& FlyTarget, float& OutYaw, float& OutPitch, float& OutRoll);
};
