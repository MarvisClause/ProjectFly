// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectFly/Pawns/BaseFlyPlane.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY(LogBaseFlyPlane);

// Sets default values
ABaseFlyPlane::ABaseFlyPlane()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetSimulatePhysics(true);
	SetRootComponent(StaticMesh);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bInheritRoll = false;
	CameraBoom->TargetArmLength = 400.0f;       // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);  // Attach the camera to the end of the boom and let the boom
	// adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;                                                // Camera does not rotate relative to arm
}

// Called to bind functionality to input
void ABaseFlyPlane::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("PitchControl", this, &ABaseFlyPlane::PitchControl);
	PlayerInputComponent->BindAxis("YawControl", this, &ABaseFlyPlane::YawControl);
	PlayerInputComponent->BindAxis("RollControl", this, &ABaseFlyPlane::RollControl);
}

void ABaseFlyPlane::AddSpeed(float Speed)
{
	ForwardSpeed = FMath::Clamp(ForwardSpeed + Speed, MinimumPlaneSpeed, MaximumPlaneSpeed);
}

float ABaseFlyPlane::GetSpeed() const
{
	return ForwardSpeed;
}

TObjectPtr<UStaticMeshComponent> ABaseFlyPlane::GetStaticMesh() const
{
	return StaticMesh;
}

void ABaseFlyPlane::BeginPlay()
{
	Super::BeginPlay();

	ForwardSpeed = MaximumPlaneSpeed;
}

void ABaseFlyPlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check plane boost
	if (StaticMesh->GetForwardVector().Z < 0.0)
	{
		if (bDiveBoost == false && !DiveCheckTimerHandle.IsValid())
		{
			// Start speed timer
			GetWorld()->GetTimerManager().SetTimer(DiveCheckTimerHandle, this, &ABaseFlyPlane::DiveCheck,
				2.0f, false);
		}
	}
	else
	{
		// Stop dive speed timer
		GetWorld()->GetTimerManager().ClearTimer(DiveCheckTimerHandle);

		// Activate dive boost
		if (bDiveBoost)
		{
			bDiveBoost = false;
			bRiseDecreaseDeactivation = true;
			GetWorld()->GetTimerManager().SetTimer(BoostDeactivationTimerHandle, this, &ABaseFlyPlane::BoostDeactivation,
				5.0f, false);
		}
	}
	// Calculation of the speed change depending on the inclination of the plane
	if (StaticMesh->GetForwardVector().Z <= 0)
	{
		AddSpeed(-StaticMesh->GetForwardVector().Z * DiveSpeedIncreaseScalar);
	}
	else
	{
		AddSpeed(-StaticMesh->GetForwardVector().Z * RiseSpeedDecreaseScalar * (int)(!bRiseDecreaseDeactivation));
	}
	
	UE_LOG(LogBaseFlyPlane, Log, TEXT("%f"), ForwardSpeed);

	// Add calculated force
	StaticMesh->AddForce(StaticMesh->GetForwardVector() * ForwardSpeed);

	// Make return to initial state after roll
	GetStaticMesh()->SetRelativeRotation(FMath::Lerp(GetStaticMesh()->GetRelativeRotation(), CameraBoom->GetForwardVector().ToOrientationRotator(), DeltaTime));
}

void ABaseFlyPlane::PitchControl(float Value)
{
	const float TargetPitchSpeed = -Value * AirControl;
	FVector PitchVector = TargetPitchSpeed * StaticMesh->GetRightVector();
	StaticMesh->AddTorqueInDegrees(PitchVector);
}

void ABaseFlyPlane::YawControl(float Value)
{
	const float TargetYawSpeed = Value * AirControl;
	FVector YawVector = TargetYawSpeed * StaticMesh->GetUpVector();
	StaticMesh->AddTorqueInDegrees(YawVector);
}

void ABaseFlyPlane::RollControl(float Value)
{
	const float TargetRollSpeed = Value * AirControl;
	FVector RollVector = TargetRollSpeed * StaticMesh->GetForwardVector();
	StaticMesh->AddTorqueInDegrees(RollVector);
}

void ABaseFlyPlane::DiveCheck()
{
	if (StaticMesh->GetForwardVector().Z < 0)
	{
		bDiveBoost = true;
	}
}

void ABaseFlyPlane::BoostDeactivation()
{
	bRiseDecreaseDeactivation = false;
}
