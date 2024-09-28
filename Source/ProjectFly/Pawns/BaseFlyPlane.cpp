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
    StaticMesh->SetUseCCD(true);

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
        StaticMesh->SetAngularDamping(5.0f);
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
    const float TargetPitchSpeed = -Value * AirControl;
    FVector PitchVector = TargetPitchSpeed * StaticMesh->GetRightVector();
    StaticMesh->AddTorqueInRadians(PitchVector, NAME_None, true);
}

void ABaseFlyPlane::YawControl(float Value)
{
    const float TargetYawSpeed = Value * AirControl;
    FVector YawVector = TargetYawSpeed * StaticMesh->GetUpVector();
    StaticMesh->AddTorqueInRadians(YawVector, NAME_None, true);
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

    // Beginning to reduce the pitch angle of the airplane after reaching a speed below a certain value
    if (ForwardSpeed <= PlaneSpeedThresholdForPitchDecline)
    {
        // Make static mesh slowly face downwards
        GetStaticMesh()->SetRelativeRotation(FMath::Lerp(StaticMesh->GetRelativeRotation(), FVector(StaticMesh->GetForwardVector().X, StaticMesh->GetForwardVector().Y, -1.0f).ToOrientationRotator(), DeltaTime / 10));
    }

    // Make return to initial state after roll
    GetStaticMesh()->SetRelativeRotation(FMath::Lerp(StaticMesh->GetRelativeRotation(), CameraBoom->GetForwardVector().ToOrientationRotator(), DeltaTime * AutoRotationPlaneSpeedScalar));

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
