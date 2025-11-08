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

	void AffectDashStamina(float Stamina);

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

	// Dash/Charge
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Glider Control - Dash", meta = (AllowPrivateAccess = "true"))
	float CurrentDashStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float MaximumDashStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float MinimumStaminaForDash = 30.0f;

	// Comsumption rate per second while charging
	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float DashStaminaConsumptionRate = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float DashMaximumStrength = 800000.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float DashMaximumForwardBoost = 30000.f;

	FTimerHandle DashForceTimer;
	float DashForceRemaining = 0.0f;
	float DashForcePerTick = 0.0f;

	// Recharges units per second
	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float DashStaminaRechargeRate = 1.0f;

	// Dash duration scaling
	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float DashMinDuration = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Dash")
	float DashMaxDuration = 1.5f;

	bool bIsChargingDash = false;
	float DashChargePercent = 0.0f;

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
	void ReleaseDash();

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
