#include "ProjectFly/Pawns/GliderPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"

AGliderPawn::AGliderPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Replace Capsule with Static Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(false);
	// Slight drag, prevents overspeed
	MeshComponent->SetLinearDamping(0.7f);
	// Dampen rotation for stability
	MeshComponent->SetAngularDamping(5.0f);
	MeshComponent->SetNotifyRigidBodyCollision(true);
	RootComponent = MeshComponent;

	// Spring Arm for camera orbit
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 450.f;
	SpringArm->bUsePawnControlRotation = false;

	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	// Decouple from parent’s rotation
	SpringArm->SetUsingAbsoluteRotation(true);

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Update tick groups for actor to handle issue with camera jittering, when camera lag in spring arm is enabled
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	MeshComponent->SetTickGroup(TG_PrePhysics);

	// Make spring arm tick after physics has settled
	SpringArm->PrimaryComponentTick.TickGroup = TG_PostPhysics;
	Camera->PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void AGliderPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MeshComponent)
	{
		MeshComponent->SetMassOverrideInKg(NAME_None, 7.0f);
	}
}

FVector AGliderPawn::GetTargetAimWorldLocation() const
{
	// Could be calculated from mouse hit on world plane
	return DesiredDirection;
}

FVector AGliderPawn::GetCurrentDirection() const
{
	return MeshComponent->GetForwardVector();
}

FRotator AGliderPawn::GetCurrentRotation() const
{
	return MeshComponent->GetComponentRotation();
}

void AGliderPawn::SetDesiredDirection(FVector WorldDirection)
{
	DesiredDirection = WorldDirection;
}

void AGliderPawn::BeginPlay()
{
	Super::BeginPlay();

	// Update camera lag speed
	StartEnablingCameraLag();

	// Add initial speed
	AffectSpeed(StartPlaneSpeed);

	// Bind the OnComponentHit event
	MeshComponent->OnComponentHit.AddDynamic(this, &AGliderPawn::OnGliderHit);
}

void AGliderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///////////////// Camera's spring arm length calculation for better view when plane turns /////////////////

	// Calculate camera lag
	UpdateCameraLagTransition(DeltaTime);

	///////////////// Speed and direction calculation /////////////////

	CalculateSpeed(DeltaTime);

	// Camera/Target update
	CameraPitch = FMath::Clamp(CameraPitch, -90.f, 90.f);
	FRotator NewRotation(CameraPitch, CameraYaw, 0.0f);
	SpringArm->SetWorldRotation(NewRotation);

	if (!bDisableAutopilot)
	{
		// Autopilot calculation
		const FVector FlyTarget = MeshComponent->GetComponentLocation() + Camera->GetForwardVector() * 1000.0f;
		DesiredDirection = FlyTarget;

		float YawInput, PitchInput, RollInput;
		RunAutopilot(FlyTarget, YawInput, PitchInput, RollInput);

		FVector Torque = FVector(
			RollInput * TurnTorque.X,
			PitchInput * TurnTorque.Y,
			YawInput * TurnTorque.Z
		);
		MeshComponent->AddTorqueInRadians(MeshComponent->GetComponentRotation().RotateVector(Torque), NAME_None, true);
	}

	// Halt application
	if (bHaltInputActive)
	{
		// Halt will cost plane forward speed
		AffectSpeed(-HaltSpeedReduction);
	}

	///////////////// Physics calculation /////////////////
	
	float Speed = MeshComponent->GetComponentVelocity().Size();
	float AoA = MeshComponent->GetForwardVector().Z;

	// Lift drops hard below stall speed
	float StallSpeed = MinimumPlaneSpeed * 1.2f;
	float LiftFactor = FMath::Clamp((Speed - StallSpeed) / StallSpeed, 0.f, 1.f);
	LiftFactor *= (1.f - FMath::Abs(AoA) * 0.7f);

	float LiftForceMag = Speed * Speed * LiftFactor * LiftCoefficientScalar;
	LiftForceMag = FMath::Min(LiftForceMag, MaxLiftForce);
	FVector LiftForce = FVector::UpVector * LiftForceMag;

	// Gravity gets strong quickly when slow
	float GravityBoost = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumPlaneSpeed, StallSpeed),
		FVector2D(1.0f, 3.5f), // up from 2.5f
		Speed
	);

	FVector GravityForce = FVector::DownVector * GravityScalar * GravityMultiplier * GravityBoost;

	// Drag: very strong when slow & high AoA
	float BaseDragCoef = 0.002f;
	float AoADragBoost = 1.f + FMath::Abs(AoA) * 3.f;
	float SpeedDragBoost = (Speed < StallSpeed) ? (2.0f - Speed / StallSpeed) : 1.f;
	float DragCoefficient = BaseDragCoef * AoADragBoost * SpeedDragBoost;

	FVector Velocity = MeshComponent->GetComponentVelocity();
	FVector DragForce = -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;

	FVector TotalForce = LiftForce + GravityForce + DragForce + (MeshComponent->GetForwardVector() * ForwardSpeed);
	MeshComponent->AddForce(TotalForce);

	///////////////// Simple Turbulence ///////////////////

	// Turbulence increases smoothly with forward speed
	float TurbulenceStrength = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumPlaneSpeed, MaximumPlaneSpeed),
		FVector2D(0.0f, 1.0f),
		ForwardSpeed
	);

	// Scale the torque magnitude
	float TurbulenceTorque = TurbulenceScalar * TurbulenceStrength;

	// Smooth Perlin noise over time
	float Time = GetWorld()->GetTimeSeconds();

	// Slightly different frequencies for each axis
	float NoiseX = FMath::PerlinNoise1D(Time * 0.8f) * TurbulenceTorque;
	float NoiseY = FMath::PerlinNoise1D(Time * 1.1f + 100.f) * TurbulenceTorque;
	float NoiseZ = FMath::PerlinNoise1D(Time * 0.6f + 200.f) * TurbulenceTorque * 0.3f;

	FVector RandomTorque = FVector(NoiseX, NoiseY, NoiseZ);

	MeshComponent->AddTorqueInRadians(RandomTorque, NAME_None, true);

	///////////////// Dash behavior ///////////////////
	if (bIsChargingDash && CurrentDashStamina > 0.0f)
	{
		DashChargePercent += (DashStaminaConsumptionRate / MaximumDashStamina) * DeltaTime;
		DashChargePercent = FMath::Clamp(DashChargePercent, 0.0f, 1.0f);

		CurrentDashStamina -= DashStaminaConsumptionRate * DeltaTime;
		CurrentDashStamina = FMath::Max(CurrentDashStamina, 0.0f);
	}

	///////////////// Stall behavior ///////////////////
	if (ForwardSpeed < StallPlaneSpeedThreshold)
	{
		// Orientation control
		FVector CurrentDir = MeshComponent->GetForwardVector();
		FVector TargetDir = -FVector::UpVector;

		// Calculate angular difference between directions
		float AngleError = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CurrentDir, TargetDir)));
		FVector RotationAxis = FVector::CrossProduct(CurrentDir, TargetDir).GetSafeNormal();

		// Scale torque depending on how large the error is
		float AlignmentStrength = FMath::Clamp(AngleError / 45.0f, 0.0f, 1.0f);

		// Damping reduce rotation if we're close to target
		float TorqueStrength = FMath::GetMappedRangeValueClamped(
			FVector2D(StallPlaneSpeedThreshold, MinimumPlaneSpeed),
			FVector2D(0, StallRotationForce),
			ForwardSpeed	) * AlignmentStrength;

		// Apply torque gradually to rotate towards target
		FVector Torque = RotationAxis * TorqueStrength;

		MeshComponent->AddTorqueInRadians(Torque, NAME_None, true);
	}
}

void AGliderPawn::CalculateSpeed(float DeltaTime)
{
	float Inclination = MeshComponent->GetForwardVector().Z;

	if (Inclination < 0)
	{
		// Dive acceleration
		float DiveFactor = FMath::Clamp(-Inclination, 0.f, 1.f);
		float DiveAcceleration = FMath::Pow(DiveFactor, 1.8f) * (DiveSpeedIncreaseScalar * 1.8f);
		AffectSpeed(DiveAcceleration * DeltaTime);

		// Only recharge when diving
		if (!bIsChargingDash)
		{
			if (Inclination < 0.0f)
			{
				float RechargeAmount = DashStaminaRechargeRate * DiveFactor * DeltaTime;

				CurrentDashStamina += RechargeAmount;
				CurrentDashStamina = FMath::Min(CurrentDashStamina, MaximumDashStamina);
			}
		}
	}
	else
	{
		// Climb penalty
		float RisePenalty = FMath::Pow(Inclination, 1.5f) * (RiseSpeedDecreaseScalar * 1.2f);
		AffectSpeed(-RisePenalty * DeltaTime);
	}
}

void AGliderPawn::OnGliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
	FVector Forward = MeshComponent->GetForwardVector();
	float HitAngleFactor = 1.0f - FMath::Abs(FVector::DotProduct(Forward, Hit.Normal));
	SpeedLoss *= FMath::Lerp(0.2f, 1.0f, HitAngleFactor);

	// Update speed
	AffectSpeed(-SpeedLoss);
}

void AGliderPawn::AffectSpeed(float Speed)
{
	ForwardSpeed = FMath::Clamp(ForwardSpeed + Speed, MinimumPlaneSpeed, MaximumPlaneSpeed);
}

void AGliderPawn::AffectDashStamina(float Stamina)
{
	CurrentDashStamina = FMath::Clamp(CurrentDashStamina + Stamina, 0.0f, MaximumDashStamina);
}

void AGliderPawn::StartRemovingCameraLag()
{
	CameraLagState = ECameraLagTransitionState::Increasing;
}

void AGliderPawn::StartEnablingCameraLag()
{
	CameraLagState = ECameraLagTransitionState::Decreasing;
}

UStaticMeshComponent* AGliderPawn::GetStaticMesh() const
{
	return MeshComponent;
}

void AGliderPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &AGliderPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AGliderPawn::LookUp);

	PlayerInputComponent->BindAxis("MovePitch", this, &AGliderPawn::MovePitch);
	PlayerInputComponent->BindAxis("MoveYaw", this, &AGliderPawn::MoveYaw);
	PlayerInputComponent->BindAxis("MoveRoll", this, &AGliderPawn::MoveRoll);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AGliderPawn::StartDash);
	PlayerInputComponent->BindAction("Dash", IE_Released, this, &AGliderPawn::ReleaseDash);
	
	PlayerInputComponent->BindAction("Halt", IE_Pressed, this, &AGliderPawn::StartHalt);
	PlayerInputComponent->BindAction("Halt", IE_Released, this, &AGliderPawn::StopHalt);
}

void AGliderPawn::Turn(float Value)
{
	CameraYaw += Value * MouseSensitivity;
}

void AGliderPawn::MovePitch(float Value)
{
	if (FMath::Abs(Value) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	DisableAutopilotTemporarily();

	float Responsiveness = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumPlaneSpeed, MaximumPlaneSpeed),
		FVector2D(PitchMinKeyResponsivenessScalar, PitchMaxKeyResponsivenessScalar),
		ForwardSpeed
	);

	FVector Torque = FVector(
		0.0f,
		Value * Responsiveness,
		0.0f
	);
	MeshComponent->AddTorqueInRadians(MeshComponent->GetComponentRotation().RotateVector(Torque), NAME_None, true);
}

void AGliderPawn::MoveYaw(float Value)
{
	if (FMath::Abs(Value) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	DisableAutopilotTemporarily();

	float Responsiveness = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumAirControl, MaximumAirControl),
		FVector2D(YawMinKeyResponsivenessScalar, YawMaxKeyResponsivenessScalar),
		ForwardSpeed
	);

	FVector Torque = FVector(
		0.0f,
		0.0f,
		Value * Responsiveness
	);
	MeshComponent->AddTorqueInRadians(MeshComponent->GetComponentRotation().RotateVector(Torque), NAME_None, true);
}

void AGliderPawn::MoveRoll(float Value)
{
	if (FMath::Abs(Value) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	DisableAggressiveTurnAngleTemporarily();

	float Responsiveness = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumPlaneSpeed, MaximumPlaneSpeed),
		FVector2D(RollMinKeyResponsivenessScalar, RollMaxKeyResponsivenessScalar),
		ForwardSpeed
	);

	FVector Torque = FVector(
		Value * Responsiveness,
		0.0f,
		0.0f
	);
	MeshComponent->AddTorqueInRadians(MeshComponent->GetComponentRotation().RotateVector(Torque), NAME_None, true);
}

void AGliderPawn::StartDash()
{
	if (CurrentDashStamina < MinimumStaminaForDash)
		return;

	// Charge dash to the minimum amount instantly
	DashChargePercent = MinimumStaminaForDash / MaximumDashStamina;
	CurrentDashStamina = FMath::Clamp(CurrentDashStamina - MinimumStaminaForDash, 0.0f, MaximumDashStamina);

	bIsChargingDash = true;
}

void AGliderPawn::ReleaseDash()
{
	if (!bIsChargingDash)
		return;

	bIsChargingDash = false;

	// Map dash duration to dash charge
	float DashDuration = FMath::Lerp(DashMinDuration, DashMaxDuration, DashChargePercent);

	// Map charge to impulse strength
	float DashStrengthApplied = FMath::Lerp(0.0, DashMaximumStrength, DashChargePercent);

	// Apply impulse
	DashForceRemaining = DashStrengthApplied;
	DashForcePerTick = DashStrengthApplied / (DashDuration / GetWorld()->GetDeltaSeconds());

	GetWorld()->GetTimerManager().SetTimer(DashForceTimer, [this]()
		{
			if (DashForceRemaining <= 0.f)
			{
				GetWorld()->GetTimerManager().ClearTimer(DashForceTimer);
				return;
			}

			MeshComponent->AddForce(MeshComponent->GetForwardVector() * DashForceRemaining);
			DashForceRemaining -= DashForcePerTick;

		}, GetWorld()->GetDeltaSeconds(), true);

	// Increase forward speed
	float DashSpeedBoost = FMath::Lerp(0.0f, DashMaximumForwardBoost, DashChargePercent);
	AffectSpeed(DashSpeedBoost);

	// Reset charge
	DashChargePercent = 0.0f;
}

void AGliderPawn::StartHalt()
{
	bHaltInputActive = true;

	// Save linear damping and velocity before halt
	LinearDampingBeforeHaltBackup = MeshComponent->GetLinearDamping();

	// Change linear damping to the one, which will be used in halt period
	MeshComponent->SetLinearDamping(HaltSpeedLinearDamping);
}

void AGliderPawn::StopHalt()
{
	bHaltInputActive = false;

	MeshComponent->SetLinearDamping(LinearDampingBeforeHaltBackup);
}

void AGliderPawn::LookUp(float Value)
{
	CameraPitch += Value * MouseSensitivity;
}

void AGliderPawn::RunAutopilot(const FVector& FlyTarget, float& OutYaw, float& OutPitch, float& OutRoll)
{
	const FTransform& ActorTransform = GetActorTransform();
	FVector LocalFlyTarget = ActorTransform.InverseTransformPosition(FlyTarget).GetSafeNormal() * TurnAngleSensitivity;

	// Pitch (Z), Yaw (Y), Roll (X)
	// Base autopilot control signals (full responsiveness)
	float BasePitch = -FMath::Clamp(LocalFlyTarget.Z, -1.0f, 1.0f);
	float BaseYaw = FMath::Clamp(LocalFlyTarget.Y, -1.0f, 1.0f);

	float AggressiveRoll = FMath::Clamp(LocalFlyTarget.Y, -1.0f, 1.0f);
	float WingsLevelRoll = GetActorRightVector().Z;

	FVector ToTarget = (FlyTarget - GetActorLocation()).GetSafeNormal();
	float AngleOffTarget = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToTarget)));

	float BlendFactor = FMath::Clamp(AngleOffTarget / AggressiveTurnAngle, 0.0f, 1.0f);
	float BaseRoll = -FMath::Lerp(WingsLevelRoll, AggressiveRoll, BlendFactor);

	// Calculate responsiveness factor [0..1] based on ForwardSpeed
	// Normalize AirControl between MinimumAirControl and MaximumAirControl to [0..1]
	float Responsiveness = FMath::GetMappedRangeValueClamped(
		FVector2D(MinimumPlaneSpeed, MaximumPlaneSpeed),
		FVector2D(MinimumAirControl, MaximumAirControl),
		ForwardSpeed
	);

	// Normal autopilot control with responsiveness scaling
	OutPitch = BasePitch * Responsiveness;
	OutYaw = BaseYaw * Responsiveness;
	OutRoll = BaseRoll * Responsiveness;
}

void AGliderPawn::DisableAutopilotTemporarily()
{
	bDisableAutopilot = true;

	GetWorld()->GetTimerManager().ClearTimer(DisableAutopilotEnableTimer);
	GetWorldTimerManager().SetTimer(
		DisableAutopilotEnableTimer,
		[this]()
		{
			bDisableAutopilot = false;
		},
		DisableAutopilotTimeout,
		false
	);
}

void AGliderPawn::DisableAggressiveTurnAngleTemporarily()
{
	if (GetWorldTimerManager().IsTimerActive(EnableAggressiveTurnAngleTimer))
	{
		return;
	}
	
	float OldAggressiveTurnValue = AggressiveTurnAngle;
	AggressiveTurnAngle = 0.0f;

	// Reset the timer each time we detect input
	GetWorld()->GetTimerManager().ClearTimer(EnableAggressiveTurnAngleTimer);
	GetWorldTimerManager().SetTimer(
		EnableAggressiveTurnAngleTimer,
		[this, OldAggressiveTurnValue]()
		{
			AggressiveTurnAngle = OldAggressiveTurnValue;
		},
		AggressiveTurnAngleDisableTimeout,
		false
	);
}

void AGliderPawn::UpdateCameraLagTransition(float DeltaTime)
{
	if (!SpringArm)
		return;

	switch (CameraLagState)
	{
	case ECameraLagTransitionState::Increasing:
	{
		// Smoothly increase towards max
		SpringArm->CameraLagSpeed = FMath::FInterpTo(
			SpringArm->CameraLagSpeed,
			MaxLagSpeed,
			DeltaTime,
			CameraLagTransitionSpeedToMax
		);

		// If reached nearly max, stop transition
		if (FMath::IsNearlyEqual(SpringArm->CameraLagSpeed, MaxLagSpeed, 1.0f))
		{
			SpringArm->CameraLagSpeed = MaxLagSpeed;
			CameraLagState = ECameraLagTransitionState::None;
		}
		break;
	}

	case ECameraLagTransitionState::Decreasing:
	{
		// Smoothly decrease towards normal
		SpringArm->CameraLagSpeed = FMath::FInterpTo(
			SpringArm->CameraLagSpeed,
			NormalLagSpeed,
			DeltaTime,
			CameraLagTransitionSpeedToNormal
		);

		// If reached nearly normal, stop transition
		if (FMath::IsNearlyEqual(SpringArm->CameraLagSpeed, NormalLagSpeed, 1.0f))
		{
			SpringArm->CameraLagSpeed = NormalLagSpeed;
			CameraLagState = ECameraLagTransitionState::None;
		}
		break;
	}

	default:
		break;
	}
}