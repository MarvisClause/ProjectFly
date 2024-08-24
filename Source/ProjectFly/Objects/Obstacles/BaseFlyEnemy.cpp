#include "BaseFlyEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProjectFly/Pawns/BaseFlyPlane.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABaseFlyEnemy::ABaseFlyEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and configure the static mesh component
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetSimulatePhysics(true);
    StaticMesh->SetEnableGravity(false);
    StaticMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    SetRootComponent(StaticMesh);

    // Create and configure the trigger area component
    TargetLockTriggerArea = CreateDefaultSubobject<USphereComponent>(TEXT("TargetLockTriggerArea"));
    TargetLockTriggerArea->SetupAttachment(StaticMesh);
    TargetLockTriggerArea->OnComponentBeginOverlap.AddDynamic(this, &ABaseFlyEnemy::OnTriggerAreaBeginOverlap);
    TargetLockTriggerArea->OnComponentEndOverlap.AddDynamic(this, &ABaseFlyEnemy::OnTriggerAreaEndOverlap);
}

void ABaseFlyEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseFlyEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Handle target locking and movement
    if (IsTargetLocked)
    {
        if (LockedTarget)
        {
            // Get the direction towards the target
            FVector DirectionToTarget = (LockedTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();

            // Calculate the desired rotation
            FRotator DesiredRotation = DirectionToTarget.Rotation();

            // Smoothly interpolate towards the desired rotation
            FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaTime, RotationSpeed);
            SetActorRotation(NewRotation);

            // Calculate the angle between the forward vector and the direction to the target
            float DotProduct = FVector::DotProduct(GetActorForwardVector(), DirectionToTarget);
            float AngleDegrees = FMath::Acos(DotProduct) * (180.0f / PI);

            // Apply force towards the target
            float FacingThreshold = 20.0f; 
            if (AngleDegrees <= FacingThreshold)
            {
                FVector Force = GetActorForwardVector() * MovementAcceleration;
                StaticMesh->AddForce(Force, NAME_None, true);
            }

            // Optional: Set the velocity to a fixed speed
            FVector Velocity = StaticMesh->GetPhysicsLinearVelocity().GetSafeNormal() * MovementSpeed;
            StaticMesh->SetPhysicsLinearVelocity(Velocity);

            // Constrain unwanted rotations
            FRotator ConstrainedRotation = GetActorRotation();
            ConstrainedRotation.Roll = 0.0f;  // Lock the Roll axis to prevent spinning
            SetActorRotation(ConstrainedRotation);

            // Optional: Apply angular damping to reduce spinning from physics
            StaticMesh->SetAngularDamping(5.0f);  // Adjust this value for the desired damping effect
            StaticMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);  // Reset angular velocity

            // Debug line to visualize the direction to the target
            DrawDebugLine(GetWorld(), GetActorLocation(), LockedTarget->GetActorLocation(), FColor::Red, false, -1.0f, 0, 1.0f);
        }
    }
    else
    {
        // Handle the target locking delay
        if (!AffectedPlanesArray.IsEmpty())
        {
            TargetLockTime += DeltaTime;

            if (TargetLockTime >= DelayBeforeTargetLock)
            {
                LockedTarget = AffectedPlanesArray[0];
                IsTargetLocked = true;
            }
        }
        else
        {
            TargetLockTime = 0.0f;
        }
    }
}

void ABaseFlyEnemy::OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ABaseFlyPlane* FlyPlane = Cast<ABaseFlyPlane>(OtherActor))
    {
        if (!AffectedPlanesArray.Contains(FlyPlane))
        {
            AffectedPlanesArray.Add(FlyPlane);
        }
    }
}

void ABaseFlyEnemy::OnTriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ABaseFlyPlane* FlyPlane = Cast<ABaseFlyPlane>(OtherActor))
    {
        AffectedPlanesArray.Remove(FlyPlane);
    }
}