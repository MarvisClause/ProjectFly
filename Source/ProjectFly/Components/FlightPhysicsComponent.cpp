#include "ProjectFly/Components/FlightPhysicsComponent.h"

#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

UFlightPhysicsComponent::UFlightPhysicsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UFlightPhysicsComponent::BeginPlay()
{
    Super::BeginPlay();

	TArray<UStaticMeshComponent*> MeshComponents;
	GetOwner()->GetComponents<UStaticMeshComponent>(MeshComponents);
	for (UStaticMeshComponent* Component : MeshComponents)
	{
		if (Component && Component->ComponentHasTag(ComponentTargetMeshTag))
		{
			TargetMeshComponent = Component;
			// Stop at first valid component
			break; 
		}
	}

	ensureAlwaysMsgf(TargetMeshComponent != nullptr, TEXT("TargetMeshComponent is not set! Class must be set for correct work of the given logic"));

	// Enable physics, but disable gravity. These are handled by our own logic
	TargetMeshComponent->SetSimulatePhysics(true);
	TargetMeshComponent->SetEnableGravity(false);
	
	// Slight drag, prevents overspeed
	TargetMeshComponent->SetLinearDamping(StaticMeshComponentLinearDampingOverride);

	// Dampen rotation for stability
	TargetMeshComponent->SetAngularDamping(StaticMeshComponentAngularDampingOverride);

	// Handle collisions
	TargetMeshComponent->SetNotifyRigidBodyCollision(true);

	// Override weight
	TargetMeshComponent->SetMassOverrideInKg(NAME_None, StaticMeshComponentWeightOverride, true);

	// Bind hit event
	TargetMeshComponent->OnComponentHit.AddDynamic(this, &UFlightPhysicsComponent::OnMeshHit);

	// Set initial speed
	AffectSpeed(StartSpeed);

	// Activate component
	Activate(true);
}

void UFlightPhysicsComponent::MovePitch(float Value)
{
	if (FMath::Abs(Value) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	float Responsiveness = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumSpeed, MaximumSpeed),
		FVector2D(MovePitchMinResponsivenessScalar, MovePitchMaxResponsivenessScalar),
		ForwardSpeed
	);

	FVector Torque = FVector(
		0.0f,
		Value * Responsiveness,
		0.0f
	);
	TargetMeshComponent->AddTorqueInRadians(TargetMeshComponent->GetComponentRotation().RotateVector(Torque), NAME_None, true);
}

void UFlightPhysicsComponent::MoveYaw(float Value)
{
	if (FMath::Abs(Value) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	float Responsiveness = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumAirControl, MaximumAirControl),
		FVector2D(MoveYawMinResponsivenessScalar, MoveYawMaxResponsivenessScalar),
		ForwardSpeed
	);

	FVector Torque = FVector(
		0.0f,
		0.0f,
		Value * Responsiveness
	);
	TargetMeshComponent->AddTorqueInRadians(TargetMeshComponent->GetComponentRotation().RotateVector(Torque), NAME_None, true);
}

void UFlightPhysicsComponent::MoveRoll(float Value)
{
	if (FMath::Abs(Value) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	float Responsiveness = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumSpeed, MaximumSpeed),
		FVector2D(MoveRollMinResponsivenessScalar, MoveRollMaxResponsivenessScalar),
		ForwardSpeed
	);

	FVector Torque = FVector(
		Value * Responsiveness,
		0.0f,
		0.0f
	);
	TargetMeshComponent->AddTorqueInRadians(TargetMeshComponent->GetComponentRotation().RotateVector(Torque), NAME_None, true);
}

void UFlightPhysicsComponent::AffectSpeed(float Speed)
{
	ForwardSpeed = FMath::Clamp(ForwardSpeed + Speed, MinimumSpeed, MaximumSpeed);
}

void UFlightPhysicsComponent::OnMeshHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	const float ImpactStrength = NormalImpulse.Size();

	float SpeedLoss = 0.0f;

	// In case of smaller impact ignore
	if (ImpactStrength < MinorImpactThreshold)
	{
		return;
	}
	// Apply minor impact
	else if (ImpactStrength < MajorImpactThreshold)
	{
		SpeedLoss = ForwardSpeed * MinorImpactPercent;
	}
	// Apply big impact
	else
	{
		SpeedLoss = ForwardSpeed * MajorImpactPercent;
	}

	// Calculate, if hit was a direct one or as scape one
	FVector Forward = TargetMeshComponent->GetForwardVector();
	float HitAngleFactor = 1.0f - FMath::Abs(FVector::DotProduct(Forward, Hit.Normal));
	SpeedLoss *= FMath::Lerp(0.2f, 1.0f, HitAngleFactor);

	// Update speed
	AffectSpeed(-SpeedLoss);
}

void UFlightPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Calculate speed
	CalculateSpeed();

	// Autopilot logic
	if (bAutopilotEnabled)
	{
		RunAutopilot();
	}

	// Physics logic
	RunPhysics();

	// Turbulence logic
	RunTurbulence();

	// Stall logic
	RunStall();
}

void UFlightPhysicsComponent::CalculateSpeed()
{
	float Inclination = TargetMeshComponent->GetForwardVector().Z;

	if (Inclination < RiseInclinationThreshold)
	{
		// Dive acceleration
		float DiveFactor = FMath::GetMappedRangeValueClamped(
			FVector2D(RiseInclinationThreshold, -1.0f),
			FVector2D(0, 1.0f),
			Inclination);
		float DiveAcceleration = FMath::Pow(DiveFactor, 1.8f) * (DiveSpeedIncreaseScalar * 1.8f);
		AffectSpeed(DiveAcceleration * GetWorld()->DeltaTimeSeconds);

		// Broadcast dive event
		OnDiveTick.Broadcast(DiveFactor);
	}
	else
	{
		// Climb penalty
		float RisePenalty = FMath::Pow(Inclination, 1.5f) * (RiseSpeedDecreaseScalar * 1.2f);
		AffectSpeed(-RisePenalty * GetWorld()->DeltaTimeSeconds);
	}
}

void UFlightPhysicsComponent::RunAutopilot()
{
	const FTransform& ActorTransform = TargetMeshComponent->GetComponentTransform();
	FVector LocalFlyTarget = ActorTransform.InverseTransformPosition(AutopilotTargetPosition).GetSafeNormal() * AutopilotTurnAngleSensitivity;

	// Pitch (Z), Yaw (Y), Roll (X)
	// Base autopilot control signals (full responsiveness)
	float BasePitch = -FMath::Clamp(LocalFlyTarget.Z, -1.0f, 1.0f);
	float BaseYaw = FMath::Clamp(LocalFlyTarget.Y, -1.0f, 1.0f);

	float AggressiveRoll = FMath::Clamp(LocalFlyTarget.Y, -1.0f, 1.0f);
	float WingsLevelRoll = TargetMeshComponent->GetRightVector().Z;

	FVector ToTarget = (AutopilotTargetPosition - TargetMeshComponent->GetComponentLocation()).GetSafeNormal();
	float AngleOffTarget = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetMeshComponent->GetForwardVector(), ToTarget)));

	float BlendFactor = FMath::Clamp(AngleOffTarget / AutopilotAggressiveTurnAngle, 0.0f, 1.0f);
	float BaseRoll = -FMath::Lerp(WingsLevelRoll, AggressiveRoll, BlendFactor);

	// Calculate responsiveness factor [0..1] based on ForwardSpeed
	// Normalize AirControl between MinimumAirControl and MaximumAirControl to [0..1]
	float Responsiveness = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumSpeed, MaximumSpeed),
		FVector2D(MinimumAirControl, MaximumAirControl),
		ForwardSpeed
	);

	// Normal autopilot control with responsiveness scaling
	float Pitch = BasePitch * Responsiveness;
	float Yaw = BaseYaw * Responsiveness;
	float Roll = BaseRoll * Responsiveness;

	// Apply autopilot torque
	FVector Torque = FVector(
		Roll * AutopilotTurnTorque.X,
		Pitch * AutopilotTurnTorque.Y,
		Yaw * AutopilotTurnTorque.Z
	);
	TargetMeshComponent->AddTorqueInRadians(TargetMeshComponent->GetComponentRotation().RotateVector(Torque), NAME_None, true);
}

void UFlightPhysicsComponent::RunPhysics()
{
	///////////////// Physics calculation /////////////////

	///////////////////////// Speed and angle of attack
	float Speed = TargetMeshComponent->GetComponentVelocity().Size();
	float AoA = TargetMeshComponent->GetForwardVector().Z;

	///////////////////////// Lift calculation. Improves at high speed
	FVector LiftForce = FVector();
	if (bLiftEnabled)
	{
		float LiftFactor = FMath::Clamp((Speed - StallSpeedThreshold) / StallSpeedThreshold, 0.f, 1.f);
		LiftFactor *= (1.f - FMath::Abs(AoA) * AoALiftPenaltyScalar);

		float LiftForceMag = Speed * Speed * LiftFactor * LiftCoefficientScalar;
		LiftForceMag = FMath::Min(LiftForceMag, MaxLiftForce);
		LiftForce = FVector::UpVector * LiftForceMag;
	}

	///////////////////////// Gravity calculation Gravity gets strong quickly when slow
	FVector GravityForce = FVector();
	if (bGravityEnabled)
	{
		float GravityBoost = FMath::GetMappedRangeValueClamped(
			FVector2D(MinimumSpeed, StallSpeedThreshold),
			FVector2D(1.0f, MaxGravityBoost),
			Speed
		);

		GravityForce = FVector::DownVector * GravityScalar * GravityBoost;
	}
	
	///////////////////////// Drag calculation. Very strong when slow & high AoA
	float AoADragBoost = 1.f + FMath::Abs(AoA) * AoADragMultiplier;
	float SpeedDragBoost = (Speed < StallSpeedThreshold) ? (MaxLowSpeedDragMultiplier - Speed / StallSpeedThreshold) : 1.f;
	float DragCoefficient = BaseDragCoefficient * AoADragBoost * SpeedDragBoost;

	FVector Velocity = TargetMeshComponent->GetComponentVelocity();
	FVector DragForce = -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;

	///////////////////////// Final force calculation
	FVector TotalForce = LiftForce + GravityForce + DragForce + (TargetMeshComponent->GetForwardVector() * ForwardSpeed);
	TargetMeshComponent->AddForce(TotalForce);
}

void UFlightPhysicsComponent::RunTurbulence()
{
	// Turbulence increases smoothly with forward speed
	float TurbulenceStrength = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumSpeed, MaximumSpeed),
		FVector2D(0.0f, 1.0f),
		ForwardSpeed
	);

	// Scale the torque magnitude
	float TurbulenceTorque = TurbulenceScalar * TurbulenceStrength;

	// Smooth Perlin noise over time
	float Time = GetWorld()->GetTimeSeconds();

	// Perlin noise for each axis using configurable frequencies and offsets
	float NoiseX = FMath::PerlinNoise1D(Time * TurbulenceFrequencyX + TurbulenceOffsetX) * TurbulenceTorque;
	float NoiseY = FMath::PerlinNoise1D(Time * TurbulenceFrequencyY + TurbulenceOffsetY) * TurbulenceTorque;
	float NoiseZ = FMath::PerlinNoise1D(Time * TurbulenceFrequencyZ + TurbulenceOffsetZ) * TurbulenceTorque;

	FVector RandomTorque = FVector(NoiseX, NoiseY, NoiseZ);

	TargetMeshComponent->AddTorqueInRadians(RandomTorque, NAME_None, true);
}

void UFlightPhysicsComponent::RunStall()
{
	if (ForwardSpeed < StallSpeedThreshold)
	{
		// Orientation control
		FVector CurrentDir = TargetMeshComponent->GetForwardVector();
		FVector TargetDir = -FVector::UpVector;

		// Calculate angular difference between directions
		float AngleError = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CurrentDir, TargetDir)));
		FVector RotationAxis = FVector::CrossProduct(CurrentDir, TargetDir).GetSafeNormal();

		// Scale torque depending on how large the error is
		float AlignmentStrength = FMath::Clamp(AngleError / 45.0f, 0.0f, 1.0f);

		// Damping reduce rotation if we're close to target
		float TorqueStrength = FMath::GetMappedRangeValueClamped(
			FVector2D(StallSpeedThreshold, MinimumSpeed),
			FVector2D(0, StallRotationForce),
			ForwardSpeed) * AlignmentStrength;

		// Apply torque gradually to rotate towards target
		FVector Torque = RotationAxis * TorqueStrength;

		TargetMeshComponent->AddTorqueInRadians(Torque, NAME_None, true);
	}
}