#include "DeathPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"

ADeathPawn::ADeathPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	// Wreck mesh
	WreckMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WreckMesh"));
	SetRootComponent(WreckMesh);

	WreckMesh->SetSimulatePhysics(true);
	WreckMesh->SetEnableGravity(true);
	WreckMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WreckMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

	// Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 350.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 15.f;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Player can only rotate camera, not pawn itself
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
}

void ADeathPawn::BeginPlay()
{
	Super::BeginPlay();

	if (AutoDestroyDelay > 0)
	{
		SetLifeSpan(AutoDestroyDelay);
	}

	if (!InitialTorque.IsZero())
	{
		WreckMesh->AddTorqueInRadians(InitialTorque, NAME_None, true);
	}
}

void ADeathPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///////////////////////// Camera/Target update
	CameraPitch = FMath::Clamp(CameraPitch, -90.f, 90.f);
	FRotator NewRotation(CameraPitch, CameraYaw, 0.0f);
	SpringArm->SetWorldRotation(NewRotation);
}

void ADeathPawn::ApplyInheritedVelocity(const FVector& Velocity)
{
	if (WreckMesh)
	{
		WreckMesh->SetPhysicsLinearVelocity(Velocity);
	}
}

void ADeathPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!bAllowCameraRotation)
		return;

	PlayerInputComponent->BindAxis("Turn", this, &ADeathPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ADeathPawn::LookUp);
}

void ADeathPawn::Turn(float Value)
{
	CameraYaw += Value * MouseSensitivity;
}

void ADeathPawn::LookUp(float Value)
{
	CameraPitch += Value * MouseSensitivity;
}
