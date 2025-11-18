#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GliderPawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UFlightPhysicsComponent;

UCLASS()
class PROJECTFLY_API AGliderPawn : public APawn
{
	GENERATED_BODY()

public:
	AGliderPawn();
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

	void AffectDashStamina(float Stamina);

	void StartRemovingCameraLag();
	void StartEnablingCameraLag();

	UStaticMeshComponent* GetStaticMesh() const;

	UFlightPhysicsComponent* AccessFlightPhysicsComponent();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	///////////////////////// Components
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	UFlightPhysicsComponent* FlightPhysicsComponent;

	///////////////////////// Input variables
	
	float CameraYaw;
	float CameraPitch;

	///////////////////////// Dash/Charge
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

	///////////////////////// Halt Settings
	bool bHaltInputActive = false;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Halt", meta = (ClampMin = 0.0f))
	float HaltSpeedReduction = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Halt", meta = (ClampMin = 0.0f))
	float HaltSpeedLinearDamping = 4.0f;

	float LinearDampingBeforeHaltBackup = 0.0f;

	///////////////////////// Camera Settings

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
	float MouseSensitivity = 1.0f;

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

	///////////////////////// Timer for roll logic control
	UPROPERTY(EditAnywhere, Category = "Glider Control - Aggressive Turn Angle Disable Timeout", meta = (ClampMin = 0.1f))
	float AggressiveTurnAngleDisableTimeout = 0.5f;
	FTimerHandle EnableAggressiveTurnAngleTimer;

	///////////////////////// Boolean variable, which controls autopilot pitch and roll
	UPROPERTY(EditAnywhere, Category = "Glider Control - Manual Control Timeout", meta = (ClampMin = 0.1f))
	float DisableAutopilotTimeout = 0.1f;
	FTimerHandle DisableAutopilotEnableTimer;

	// Autopilot state control
	void DisableAutopilotTemporarily();

	// Roll controls
	void DisableAggressiveTurnAngleTemporarily();

	// Camera
	void UpdateCameraLagTransition(float DeltaTime);

	// Dash
	void StartDash();
	void ReleaseDash();
	void ChargeDashTick(float DiveFactor);

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

	// Handlers for events
	UFUNCTION()
	void OnDiveTickHandler(float DiveFactor);
};
