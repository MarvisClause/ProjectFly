#include "ProjectFly/Pawns/GliderPawn.h"
#include "ProjectFly/Pawns/DeathPawn.h"
#include "ProjectFly/Components/FlightPhysicsComponent.h"
#include "ProjectFly/Components/HealthComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

AGliderPawn::AGliderPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Main scene component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	// Mesh component, main moving element of glider pawn
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetNotifyRigidBodyCollision(true);

	// Camera focus scene component
	CameraFocusSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CameraFocusSceneComponent"));
	CameraFocusSceneComponent->SetupAttachment(RootComponent);

	// Spring Arm for camera orbit
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CameraFocusSceneComponent);
	SpringArm->TargetArmLength = 450.f;
	SpringArm->bEnableCameraLag = false;

	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Flight physics component
	FlightPhysicsComponent = CreateDefaultSubobject<UFlightPhysicsComponent>(TEXT("FlightPhysics"));

	// Health component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
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

	// Subscribe to event
	FlightPhysicsComponent->OnDiveTick.AddDynamic(this, &AGliderPawn::OnDiveTickHandler );
	FlightPhysicsComponent->OnMeshComponentHit.AddDynamic( this, &AGliderPawn::OnMeshComponentHitHandler );
	FlightPhysicsComponent->OnMeshComponentMinorHit.AddDynamic(this, &AGliderPawn::OnMeshComponentMinorHitHandler);
	FlightPhysicsComponent->OnMeshComponentMajorHit.AddDynamic(this, &AGliderPawn::OnMeshComponentMajorHitHandler);
	HealthComponent->OnDeath.AddDynamic( this, &AGliderPawn::OnDeathHandler );
}

void AGliderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///////////////////////// Camera/Target update
	CameraPitch = FMath::Clamp(CameraPitch, -90.f, 90.f);
	FRotator NewRotation(CameraPitch, CameraYaw, 0.0f);
	CameraFocusSceneComponent->SetWorldRotation(NewRotation);

	FVector TargetLocation = MeshComponent->GetComponentLocation();
	FVector CurrentLocation = CameraFocusSceneComponent->GetComponentLocation();

	// Critically damped spring formula
	FVector Delta = TargetLocation - CurrentLocation;
	FVector Acceleration = Delta * CameraSpringStiffness - CameraVelocity * CameraSpringDamping;
	CameraVelocity += Acceleration * DeltaTime;

	// Clamp max movement to prevent overshoot at high speed
	FVector MoveStep = CameraVelocity * DeltaTime;
	float MaxMoveThisFrame = CameraMaxMovePerSecond * DeltaTime;
	MoveStep = MoveStep.GetClampedToMaxSize(MaxMoveThisFrame);

	CameraFocusSceneComponent->SetWorldLocation(CurrentLocation + MoveStep);

	FlightPhysicsComponent->SetTargetAutopilotPosition(MeshComponent->GetComponentLocation() + Camera->GetForwardVector() * 1000.0f);

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

void AGliderPawn::OnMeshComponentHitHandler()
{
	HealthComponent->ApplyDamage(RegularHitDamage);
}

void AGliderPawn::OnMeshComponentMinorHitHandler()
{
	HealthComponent->ApplyDamage(MinorHitDamage);
}

void AGliderPawn::OnMeshComponentMajorHitHandler()
{
	HealthComponent->ApplyDamage(MajorHitDamage);
}

void AGliderPawn::OnDeathHandler()
{
	if (!DeathPawnClass) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADeathPawn* Wreck = GetWorld()->SpawnActor<ADeathPawn>(
		DeathPawnClass,
		MeshComponent->GetComponentTransform(),
		Params
	);

	if (Wreck && MeshComponent)
	{
		Wreck->ApplyInheritedVelocity(MeshComponent->GetComponentVelocity());
	}

	// Switch control to newly spawned pawn
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetViewTargetWithBlend(Wreck, 1.5f);
		PC->Possess(Wreck);
	}

	Destroy();
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

void AGliderPawn::Turn(float Value)
{
	CameraYaw += Value * MouseSensitivity;
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