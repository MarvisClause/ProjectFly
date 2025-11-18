#include "ProjectFly/Pawns/GliderPawn.h"
#include "ProjectFly/Components/FlightPhysicsComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

AGliderPawn::AGliderPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Replace Capsule with Static Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
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
	PrimaryActorTick.TickGroup = TG_DuringPhysics;
	MeshComponent->SetTickGroup(TG_DuringPhysics);

	// Make spring arm tick after physics has settled
	SpringArm->PrimaryComponentTick.TickGroup = TG_PostPhysics;
	Camera->PrimaryComponentTick.TickGroup = TG_PostPhysics;

	// Flight physics component
	FlightPhysicsComponent = CreateDefaultSubobject<UFlightPhysicsComponent>(TEXT("FlightPhysics"));
}

void AGliderPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MeshComponent)
	{
		MeshComponent->SetMassOverrideInKg(NAME_None, 7.0f);
	}
}

void AGliderPawn::BeginPlay()
{
	Super::BeginPlay();

	// Update camera lag speed
	StartEnablingCameraLag();

	// Subscribe to event
	FlightPhysicsComponent->OnDiveTick.AddDynamic(this, &AGliderPawn::OnDiveTickHandler );
}

void AGliderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///////////////////////// Camera/Target update
	CameraPitch = FMath::Clamp(CameraPitch, -90.f, 90.f);
	FRotator NewRotation(CameraPitch, CameraYaw, 0.0f);
	SpringArm->SetWorldRotation(NewRotation);
	FlightPhysicsComponent->SetTargetAutopilotPosition(MeshComponent->GetComponentLocation() + Camera->GetForwardVector() * 1000.0f);

	///////////////////////// Calculate camera lag
	UpdateCameraLagTransition(DeltaTime);

	///////////////////////// Halt application
	if (bHaltInputActive)
	{
		// Halt will cost plane forward speed
		FlightPhysicsComponent->AffectSpeed(-HaltSpeedReduction);
	}

	///////////////////////// Dash behavior 
	if (bIsChargingDash && CurrentDashStamina > 0.0f)
	{
		DashChargePercent += (DashStaminaConsumptionRate / MaximumDashStamina) * DeltaTime;
		DashChargePercent = FMath::Clamp(DashChargePercent, 0.0f, 1.0f);

		CurrentDashStamina -= DashStaminaConsumptionRate * DeltaTime;
		CurrentDashStamina = FMath::Max(CurrentDashStamina, 0.0f);
	}
	else if (bIsChargingDash && CurrentDashStamina <= 0)
	{
		ReleaseDash();
	}
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

UFlightPhysicsComponent* AGliderPawn::AccessFlightPhysicsComponent()
{
	return FlightPhysicsComponent;
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

	FlightPhysicsComponent->MovePitch(Value);
}

void AGliderPawn::MoveYaw(float Value)
{
	if (FMath::Abs(Value) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	DisableAutopilotTemporarily();

	FlightPhysicsComponent->MoveYaw(Value);
}

void AGliderPawn::MoveRoll(float Value)
{
	if (FMath::Abs(Value) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	DisableAggressiveTurnAngleTemporarily();

	FlightPhysicsComponent->MoveRoll(Value);
}

void AGliderPawn::OnDiveTickHandler(float DiveFactor)
{
	ChargeDashTick(DiveFactor);
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
	FlightPhysicsComponent->AffectSpeed(DashSpeedBoost);

	// Reset charge
	DashChargePercent = 0.0f;
}

void AGliderPawn::ChargeDashTick(float DiveFactor)
{
	// Only recharge when diving and halt is not activated
	if (!bIsChargingDash && !bHaltInputActive)
	{
		float RechargeAmount = DashStaminaRechargeRate * DiveFactor * GetWorld()->DeltaTimeSeconds;

		CurrentDashStamina += RechargeAmount;
		CurrentDashStamina = FMath::Min(CurrentDashStamina, MaximumDashStamina);
	}
}

void AGliderPawn::StartHalt()
{
	bHaltInputActive = true;

	// Save linear damping and velocity before halt
	LinearDampingBeforeHaltBackup = FlightPhysicsComponent->GetStaticMeshComponentLinearDampingOverride();

	// Change linear damping to the one, which will be used in halt period
	FlightPhysicsComponent->SetStaticMeshComponentLinearDampingOverride(HaltSpeedLinearDamping);
}

void AGliderPawn::StopHalt()
{
	bHaltInputActive = false;

	// Return original linear damping value
	FlightPhysicsComponent->SetStaticMeshComponentLinearDampingOverride(LinearDampingBeforeHaltBackup);
}

void AGliderPawn::LookUp(float Value)
{
	CameraPitch += Value * MouseSensitivity;
}

void AGliderPawn::DisableAutopilotTemporarily()
{
	FlightPhysicsComponent->SetAutopilotState(false);

	GetWorld()->GetTimerManager().ClearTimer(DisableAutopilotEnableTimer);
	GetWorldTimerManager().SetTimer(
		DisableAutopilotEnableTimer,
		[this]()
		{
			FlightPhysicsComponent->SetAutopilotState(true);
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
	
	float OldAggressiveTurnValue = FlightPhysicsComponent->GetAutopilotAggressiveTurnAngle();
	FlightPhysicsComponent->SetAutopilotAggressiveTurnAngle(0.0f);

	// Reset the timer each time we detect input
	GetWorld()->GetTimerManager().ClearTimer(EnableAggressiveTurnAngleTimer);
	GetWorldTimerManager().SetTimer(
		EnableAggressiveTurnAngleTimer,
		[this, OldAggressiveTurnValue]()
		{
			FlightPhysicsComponent->SetAutopilotAggressiveTurnAngle(OldAggressiveTurnValue);
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