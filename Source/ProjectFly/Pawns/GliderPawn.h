#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GliderPawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UFlightPhysicsComponent;
class UHealthComponent;
class ADeathPawn;

UCLASS()
class PROJECTFLY_API AGliderPawn : public APawn
{
	GENERATED_BODY()

public:
	AGliderPawn();
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

	void AffectDashStamina(float Stamina);

	UStaticMeshComponent* GetStaticMesh() const;

	UFlightPhysicsComponent* AccessFlightPhysicsComponent();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	///////////////////////// Components

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* CameraFocusSceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	UFlightPhysicsComponent* FlightPhysicsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glider Control - Components", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	///////////////////////// Death pawn

	UPROPERTY(EditAnywhere, Category = "Glider Control - Death Pawn")
	TSubclassOf<ADeathPawn> DeathPawnClass;

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

	///////////////////////// Damage Settings

	UPROPERTY(EditAnywhere, Category = "Glider Control - Damage", meta = (ClampMin = 0.0f))
	float MinorHitDamage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Damage", meta = (ClampMin = 0.0f))
	float MajorHitDamage = 55.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Damage", meta = (ClampMin = 0.0f))
	float RegularHitDamage = 1.0f;

	///////////////////////// Camera Settings

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
	float MouseSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
	float BaseFOV = 90.f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
	float MaxFOV = 120.f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
	float DashFOVScalar = 10.f;

	UPROPERTY(EditAnywhere, Category = "Glider Control - Camera", meta = (ClampMin = 0.0f))
	float FOVInterpSpeed = 8.f;

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

	// Dash
	void StartDash();
	void ReleaseDash();
	void ChargeDashTick(float DiveFactor);

	// Halt
	void StartHalt();
	void StopHalt();
	
	// Free Look
	void StartFreeLook();
	void StopFreeLook();

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

	UFUNCTION()
	void OnMeshComponentHitHandler();
	UFUNCTION()
	void OnMeshComponentMinorHitHandler();
	UFUNCTION()
	void OnMeshComponentMajorHitHandler();

	UFUNCTION()
	void OnDeathHandler();
};
