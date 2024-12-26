// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectFly/Pawns/BaseFlyPlane.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY(LogBaseFlyPlane);

ABaseFlyPlane::ABaseFlyPlane()
{
    PrimaryActorTick.bCanEverTick = true;

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetSimulatePhysics(true);
    StaticMesh->SetMassOverrideInKg(NAME_None, 0.1f);   // Set the plane's mass (lower value for lighter feel)
    StaticMesh->SetLinearDamping(5.0f);                        // Set the plane's linear damping (lower value for less drag)
    StaticMesh->SetAngularDamping(5.0f);                       // Set the plane's angular damping (lower value for less rotational drag)
    StaticMesh->SetNotifyRigidBodyCollision(true);

    SetRootComponent(StaticMesh);

    FocusSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FocusPoint"));
    FocusSceneComponent->SetupAttachment(StaticMesh);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(FocusSceneComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = false;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

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

    // The less speed we have the less control user has over it's plane
    AirControl = FMath::GetMappedRangeValueClamped(
        FVector2D(PlaneSpeedThresholdForPitchDecline, MaximumPlaneSpeed),
        FVector2D(MinimumAirControl, MaximumAirControl),
        ForwardSpeed
    );
}

float ABaseFlyPlane::GetSpeed() const
{
    return ForwardSpeed;
}

UStaticMeshComponent* ABaseFlyPlane::GetStaticMesh() const
{
    return StaticMesh;
}

void ABaseFlyPlane::BeginPlay()
{
    Super::BeginPlay();

    // Add initial speed
    AddSpeed(StartPlaneSpeed);

    // Bind the OnComponentHit event
    StaticMesh->OnComponentHit.AddDynamic(this, &ABaseFlyPlane::OnPlaneHit);
}

void ABaseFlyPlane::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CalculateSpeed(DeltaTime);
    CalculateRotation(DeltaTime);
    UpdateCamera();

    if (ForwardSpeed < 100.0f)
    {
        StaticMesh->SetMassScale(NAME_None, 1000.0f);
        StaticMesh->SetLinearDamping(0.01f);
        StaticMesh->SetAngularDamping(2.0f);
    }
    else
    {
        StaticMesh->SetMassScale(NAME_None, 0.1f);
        StaticMesh->SetLinearDamping(5.0f);
        StaticMesh->SetAngularDamping(5.0f);
    }

    UE_LOG(LogBaseFlyPlane, Log, TEXT("%f"), ForwardSpeed);
}

void ABaseFlyPlane::PitchControl(float Value)
{
    // Calculate speed ratio (scale between 0 and 1 based on speed)
    const float SpeedRatio = FMath::GetMappedRangeValueClamped(
        FVector2D(MinimumPlaneSpeed, MaximumPlaneSpeed),
        FVector2D(0.0f, 1.0f),
        ForwardSpeed
    );

    // Adjust Value based on SpeedRatio
    // Positive Value (nose up) is scaled by SpeedRatio; Negative Value (nose down) is less affected
    float AdjustedValue = Value > 0.0f ? Value * SpeedRatio : Value * FMath::Lerp(1.0f, 1.5f, 1.0f - SpeedRatio);

    // Calculate the target pitch speed and apply torque
    const float TargetPitchSpeed = -AdjustedValue * AirControl;
    FVector PitchVector = TargetPitchSpeed * StaticMesh->GetRightVector();
    StaticMesh->AddTorqueInRadians(PitchVector, NAME_None, true);
}

void ABaseFlyPlane::YawControl(float Value)
{
    const float TargetYawSpeed = Value * AirControl;
    FVector YawVector = TargetYawSpeed * StaticMesh->GetUpVector();
    StaticMesh->AddTorqueInRadians(YawVector, NAME_None, true);
	
    RollControl(-Value);
}

void ABaseFlyPlane::RollControl(float Value)
{
    const float TargetRollSpeed = Value * AirControl;
    FVector RollVector = TargetRollSpeed * StaticMesh->GetForwardVector();
    StaticMesh->AddTorqueInRadians(RollVector, NAME_None, true);
}

void ABaseFlyPlane::CalculateSpeed(float DeltaTime)
{
    // Calculation of the speed change depending on the inclination of the plane
    if (StaticMesh->GetForwardVector().Z <= 0)
    {
        AddSpeed(-StaticMesh->GetForwardVector().Z * DiveSpeedIncreaseScalar);
    }
    else
    {
        AddSpeed(-StaticMesh->GetForwardVector().Z * RiseSpeedDecreaseScalar);
    }
    StaticMesh->AddForce(StaticMesh->GetForwardVector() * ForwardSpeed);
}

void ABaseFlyPlane::CalculateRotation(float DeltaTime)
{
    const float TurbulenceValue = TurbulenceFactor / 10000.0f;

    // Calculate turbulence
    const FVector CurrentVelocity = StaticMesh->GetComponentVelocity();
    const FVector Turbulence = FVector(
        FMath::FRandRange(-1.0f, 1.0f),
        FMath::FRandRange(-1.0f, 1.0f),
        FMath::FRandRange(-1.0f, 1.0f)
    ) * TurbulenceValue;

    // Apply forces
    StaticMesh->AddForce(Turbulence);

    // Apply rotational forces
    const FVector PitchVector = StaticMesh->GetRightVector() * TurbulenceValue * FMath::FRandRange(-1.0f, 1.0f) * ForwardSpeed;
    const FVector YawVector = StaticMesh->GetUpVector() * TurbulenceValue * FMath::FRandRange(-1.0f, 1.0f) * ForwardSpeed;
    const FVector RollVector = StaticMesh->GetForwardVector() * TurbulenceValue * FMath::FRandRange(-1.0f, 1.0f) * ForwardSpeed;
    StaticMesh->AddTorqueInRadians(PitchVector, NAME_None, true);
    StaticMesh->AddTorqueInRadians(YawVector, NAME_None, true);
    StaticMesh->AddTorqueInRadians(RollVector, NAME_None, true);

    // Adjust pitch angle based on speed
    const float SpeedRatio = FMath::GetMappedRangeValueClamped(
        FVector2D(MinimumPlaneSpeed, MaximumPlaneSpeed),
        FVector2D(0.0f, 1.0f),
        ForwardSpeed
    );
    const float DownwardAngleFactor = FMath::Lerp(0.0f, -DownwardAngle, 1.0f - SpeedRatio);
    FRotator DownwardRotation = FRotator(DownwardAngleFactor, 0.0f, 0.0f);

    // If plane is diving, don't change it's rotation by downward rotation effect,
	// otherwise it will start to rise instead
    if (StaticMesh->GetRelativeRotation().Pitch <= DownwardRotation.Pitch)
    {
        DownwardRotation = StaticMesh->GetRelativeRotation();
    }

    // Interpolate towards the downward-facing rotation
    FRotator TargetRotation = StaticMesh->GetRelativeRotation();
    TargetRotation.Pitch = FMath::Lerp(TargetRotation.Pitch, DownwardRotation.Pitch, DeltaTime * 0.5f);
    StaticMesh->SetRelativeRotation(TargetRotation);

    // Return to initial state after roll
    FRotator FinalRotation = FMath::Lerp(TargetRotation, CameraBoom->GetForwardVector().ToOrientationRotator(), DeltaTime * AutoRotationPlaneSpeedScalar);
    StaticMesh->SetRelativeRotation(FinalRotation);
}

void ABaseFlyPlane::UpdateCamera()
{
    // Update camera boom length based on forward speed
    const float SpeedPercentage = FMath::GetMappedRangeValueClamped(FVector2D(MinimumPlaneSpeed, MaximumPlaneSpeed), FVector2D(0.0f, 1.0f), ForwardSpeed);
    const float CameraBoomLength = FMath::Lerp(MinimumCameraBoomLength, MaximumCameraBoomLength, SpeedPercentage);
    CameraBoom->TargetArmLength = CameraBoomLength;
}

void ABaseFlyPlane::OnPlaneHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // Decrease speed 
    ForwardSpeed -= ForwardSpeed / 2;
}
