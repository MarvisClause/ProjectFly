#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlightPhysicsComponent.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiveTick, float, DiveFactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeshComponentHit);

// Flight physics component is a complex logic, which tries to imitate flying mechanics and apply them to mesh component.
// It is expectd, that given mesh component, won't utilize physic simulation and it will be handled by flight physics component itself.
// This is not the best approach, but gives closest feeling to the flying and also allows itself to extend this logic on other objects.
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTFLY_API UFlightPhysicsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
	UFlightPhysicsComponent();

	// Used for operating with input in case of manual control. Doesn't change autopilot target
    void MovePitch(float Value);
    void MoveYaw(float Value);
	void MoveRoll(float Value);

	// Sets target for autopilot to rotate the object
	void SetTargetAutopilotPosition(FVector NewTargetAutopilotPosition) { AutopilotTargetPosition = NewTargetAutopilotPosition; }

	// Affects speed from outter source
    void AffectSpeed(float Speed);
	float GetForwardSpeed() const { return ForwardSpeed; }
	float GetMinimumSpeed() const { return MinimumSpeed; }
	float GetMaximumSpeed() const { return MaximumSpeed; }
	float GetSpeedPercentage() const { return (ForwardSpeed - MinimumSpeed) / (MaximumSpeed - MinimumSpeed);}

	// Allows outside system to eanble or disable specific states of flight physics component
	void SetGravityEnabled(bool bIsEnabled) { bGravityEnabled = bIsEnabled; }
	void SetLiftEnabled(bool bIsEnabled) { bLiftEnabled = bIsEnabled; }

	bool GetAutopilotState() const { return bAutopilotEnabled; }
	void SetAutopilotState(bool bIsEnabled) { bAutopilotEnabled = bIsEnabled; }

	// Autopilot aggressive turn angle
	float GetAutopilotAggressiveTurnAngle() const { return AutopilotAggressiveTurnAngle; }
	void SetAutopilotAggressiveTurnAngle(float NewAutopilotAggressiveTurnAngle) { AutopilotAggressiveTurnAngle = NewAutopilotAggressiveTurnAngle; }

	// Static mesh component linear damping override
	float GetStaticMeshComponentLinearDampingOverride() const { return StaticMeshComponentLinearDampingOverride; }
	void SetStaticMeshComponentLinearDampingOverride(float NewStaticMeshComponentLinearDampingOverride) { StaticMeshComponentLinearDampingOverride = NewStaticMeshComponentLinearDampingOverride; TargetMeshComponent->SetLinearDamping(StaticMeshComponentLinearDampingOverride);	}

	// Handle collision
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	// Target mesh component class, which will be found in the owner actor
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Setup")
	FName ComponentTargetMeshTag = "FlightMesh";

	// Broadcasts when dive is happening
	UPROPERTY(BlueprintAssignable, Category = "Flight Physics - Setup")
	FOnDiveTick OnDiveTick;

	// Broadcast when hit has happened
	UPROPERTY(BlueprintAssignable, Category = "Flight Physics - Setup")
	FOnMeshComponentHit OnMeshComponentHit;
	UPROPERTY(BlueprintAssignable, Category = "Flight Physics - Setup")
	FOnMeshComponentHit OnMeshComponentMinorHit;
	UPROPERTY(BlueprintAssignable, Category = "Flight Physics - Setup")
	FOnMeshComponentHit OnMeshComponentMajorHit;

protected:
    virtual void BeginPlay() override;
    
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	///////////////////////// Flying logic

	// Calculates change of speed from inclination 
	void CalculateSpeed();

	void RunAutopilot();

	void RunPhysics();

	void RunTurbulence();

	void RunStall();

	///////////////////////// Move responsiveness. Only affect according move methods 

	UPROPERTY(EditAnywhere, Category = "Flight Physics - Control Response", meta = (ClampMin = 0.0f))
	float MovePitchMinResponsivenessScalar = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Control Response", meta = (ClampMin = 0.0f))
	float MovePitchMaxResponsivenessScalar = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Flight Physics - Control Response", meta = (ClampMin = 0.0f))
	float MoveYawMinResponsivenessScalar = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Control Response", meta = (ClampMin = 0.0f))
	float MoveYawMaxResponsivenessScalar = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Flight Physics - Control Response", meta = (ClampMin = 0.0f))
	float MoveRollMinResponsivenessScalar = 15.0f;
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Control Response", meta = (ClampMin = 0.0f))
	float MoveRollMaxResponsivenessScalar = 20.0f;

	///////////////////////// Autopilot/Turn logic
	
	// Defines position, where autopilot should head towards
	FVector AutopilotTargetPosition;
	
	// Defines how sharp is the turning for the object
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turn Control", meta = (ClampMin = 0.0f))
	float AutopilotTurnAngleSensitivity = 1.0f;
	
	// Defines angle which will be used for aggressive turning
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turn Control", meta = (ClampMin = 0.0f))
	float AutopilotAggressiveTurnAngle = 10.0f;
	
	// Defines force with which object will rotate by autopilot
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turn Control")
	FVector AutopilotTurnTorque = FVector(45.f, 25.f, 45.f);

	///////////////////////// Lift logic

	// How powerful will the lift be
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Lift Control", meta = (ClampMin = 0.0f))
	float LiftCoefficientScalar = 0.003f;

	// Maximimum lift force
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Lift Control", meta = (ClampMin = 0.0f))
	float MaxLiftForce = 5000.0f;

	// How much nose angle affects lift (0 = no effect, 1 = full effect)
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Lift Control", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AoALiftPenaltyScalar = 0.7f;

	///////////////////////// Stall logic

	// Defines forward speed value, after which object will start to fall down 
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Stall Control", meta = (ClampMin = 0.0f))
	float StallSpeedThreshold = 30000.0f;

	// How powerful will the stall push object downwards
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Stall Control", meta = (ClampMin = 0.0f))
	float StallRotationForce = 15.0f;

	///////////////////////// Speed logic

	// Forward speed of the object
	// This is the main variable, which defines movement of the object
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ForwardSpeed = 0.0f;

	// Minimum speed
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Speed Control", meta = (ClampMin = 0.0f))
	float MinimumSpeed = 100000.0f;

	// Maximum speed
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Speed Control", meta = (ClampMin = 0.0f))
	float MaximumSpeed = 160000.0f;

	// Start speed
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Speed Control", meta = (ClampMin = 0.0f))
	float StartSpeed = 80000.0f;

	// How much speed will object gain while diving
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Speed Control", meta = (ClampMin = 0.0f))
	float DiveSpeedIncreaseScalar = 30000.0f;

	// How much speed will object lose while rising
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Speed Control", meta = (ClampMin = 0.0f))
	float RiseSpeedDecreaseScalar = 32000.0f;

	// Defines angle after which inclination is considered as a rise. Otherwise, it is considered as dive and increases object speed
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Speed Control", meta = (ClampMin = -1.0f, ClampMax = 1.0f))
	float RiseInclinationThreshold = 0.0f;

	///////////////////////// Air control logic

	// Minimum air control. Corresponds with minimal speed. Affects object's rotation responsiveness
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Air Control", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float MinimumAirControl = 0.3f;

	// Maximum air control. Corresponds with maximum speed. Affects object's rotation responsiveness
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Air Control", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float MaximumAirControl = 1.0f;

	///////////////////////// Gravity logic

	// Gravity boost at stall speed
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Gravity Control", meta = (ClampMin = "1.0", ClampMax = "10.0"))
	float MaxGravityBoost = 3.5f;

	// How powerful is the gravity
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Gravity Control", meta = (ClampMin = 0.0f))
	float GravityScalar = 2500.0f;

	///////////////////////// Drag logic
	
	// Base drag coefficient applied to all velocity
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Drag", meta = (ClampMin = 0.0f))
	float BaseDragCoefficient = 0.002f;

	// How strongly angle of attack (AoA) increases drag
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Drag", meta = (ClampMin = 0.0f))
	float AoADragMultiplier = 3.0f;

	// Maximum extra drag applied at very low speed
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Drag", meta = (ClampMin = 0.0f))
	float MaxLowSpeedDragMultiplier = 2.0f;

	///////////////////////// Turbulence logic
	
	// Maximum turbulence multiplier
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turbulence", meta = (ClampMin = 0.0f))
	float TurbulenceScalar = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turbulence", meta = (ClampMin = 0.0f), meta = (ClampMax = 1.0f))
	float MinimalTurbulence = 0.0f;

	// Frequency multipliers for each axis (affects speed of Perlin noise change)
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turbulence", meta = (ClampMin = 0.0f))
	float TurbulenceFrequencyX = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turbulence", meta = (ClampMin = 0.0f))
	float TurbulenceFrequencyY = 1.1f;

	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turbulence", meta = (ClampMin = 0.0f))
	float TurbulenceFrequencyZ = 0.6f;

	// Offset for Perlin noise to make each axis independent
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turbulence")
	float TurbulenceOffsetX = 0.f;

	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turbulence")
	float TurbulenceOffsetY = 100.f;

	UPROPERTY(EditAnywhere, Category = "Flight Physics - Turbulence")
	float TurbulenceOffsetZ = 200.f;

	///////////////////////// Collision logic

	// Impact threshold for configuring object
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Impact Control", meta = (ClampMin = 0.0f))
	float MinorImpactThreshold = 300.0f;
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Impact Control", meta = (ClampMin = 0.0f))
	float MajorImpactThreshold = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Flight Physics - Impact Control", meta = (ClampMin = 0.0f), meta = (ClampMax = 1.0f))
	float MinorImpactPercent = 0.3f;
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Impact Control", meta = (ClampMin = 0.0f), meta = (ClampMax = 1.0f))
	float MajorImpactPercent = 0.8f;

	///////////////////////// Weight alteration logic
	// This is a little bit "magic" thing. We have found good balance between weight and force, which gives the best
	// feeling from flight. Mesh component data is being overriden in the begin play. These values can be changed.
	
	// Overrides linear damping in mesh component
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Physics Override", meta = (ClampMin = 0.0f))
	float StaticMeshComponentLinearDampingOverride = 0.7f;

	// Overrides angular damping in mesh component
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Physics Override", meta = (ClampMin = 0.0f))
	float StaticMeshComponentAngularDampingOverride = 5.0f;

	// Weight overriding
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Physics Override", meta = (ClampMin = 0.0f))
	float StaticMeshComponentWeightOverride = 7.0f;

	///////////////////////// Utility Variables

	// Defines, if gravity is calculated in physics
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Utility")
	bool bGravityEnabled = false;

	// Defines, if lift is calculated in physics
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Utility")
	bool bLiftEnabled = false;

	// Defines, if autopilot is enabled
	UPROPERTY(EditAnywhere, Category = "Flight Physics - Utility")
	bool bAutopilotEnabled = true;

	// Main working mesh component, which is affected by everything defined by this component
	UPROPERTY()
    UStaticMeshComponent* TargetMeshComponent = nullptr;
};