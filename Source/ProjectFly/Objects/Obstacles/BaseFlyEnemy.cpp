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
        FVector TargetLocation = LockedTarget->GetActorLocation();
        FVector DirectionToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();
        float DistanceToTarget = FVector::Dist(TargetLocation, GetActorLocation());

        switch (CurrentState)
        {
        case EEnemyState::Follow:
            {
                FollowTarget(DirectionToTarget, DeltaTime);

                FVector StartLocation = GetActorLocation();
                FVector EndLocation = LockedTarget->GetActorLocation();

                FHitResult HitResult;
                FCollisionQueryParams CollisionParams;
                CollisionParams.AddIgnoredActor(this); // Ignore self

                if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams))
                {
                    // Check if we hit the locked target
                    if (IsValid(HitResult.GetActor()) && HitResult.GetActor() == LockedTarget)
                    {
                        if (DistanceToTarget < ChargeTriggerRadius)
                        {
                            CurrentState = EEnemyState::Aim;
                            AimStartTime = GetWorld()->GetTimeSeconds();
                        }
                    }
                }
            }
            break;
        case EEnemyState::Aim:
            AimAtTarget(DirectionToTarget, DeltaTime);
            if (GetWorld()->GetTimeSeconds() - AimStartTime >= AimTime)
            {
                StaticMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
                CurrentState = EEnemyState::Charge;
                ChargeStartTime = GetWorld()->GetTimeSeconds();
            }
            break;

        case EEnemyState::Charge:
            ChargeTowardsPosition(DirectionToTarget, DeltaTime);
            if (GetWorld()->GetTimeSeconds() - ChargeStartTime >= ChargeTime)
            {
                CurrentState = EEnemyState::Follow;
            }
            break;
        }
    }
    else
    {
        // Handle the target locking delay
        if (!TargetPlanesArray.IsEmpty())
        {
            TargetLockTime += DeltaTime;

            if (TargetLockTime >= DelayBeforeTargetLock)
            {
                // Check, if any plane can be hit with raycast, thus being visible
                for (int I = 0; I < TargetPlanesArray.Num(); ++I)
                {
                    // Get the target in the array
                    AActor* Target = TargetPlanesArray[I];

                    // Calculate the direction to the target (player)
                    FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

                    // Calculate the distance to the target
                    float DistanceToTarget = FVector::Dist(Target->GetActorLocation(), GetActorLocation());

                    // Perform a raycast to check if there is a clear path to the target
                    FHitResult HitResult;
                    FVector StartLocation = GetActorLocation();
                    FVector EndLocation = Target->GetActorLocation();

                    // Set up the query parameters
                    FCollisionQueryParams QueryParams;
                    QueryParams.AddIgnoredActor(this); // Ignore the scissors itself

                    // Perform the line trace (raycast)
                    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);

                    if (bHit && HitResult.GetActor() == Target)
                    {
                        LockedTarget = TargetPlanesArray[I];
                        IsTargetLocked = true;
                    }
                }

                TargetLockTime = 0.0;
            }
        }
    }
}

void ABaseFlyEnemy::OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ABaseFlyPlane* FlyPlane = Cast<ABaseFlyPlane>(OtherActor))
    {
        if (!TargetPlanesArray.Contains(FlyPlane))
        {
            TargetPlanesArray.Add(FlyPlane);
        }
    }
}

void ABaseFlyEnemy::OnTriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ABaseFlyPlane* FlyPlane = Cast<ABaseFlyPlane>(OtherActor))
    {
        TargetPlanesArray.Remove(FlyPlane);
    }
}

void ABaseFlyEnemy::FollowTarget(const FVector& DirectionToTarget, float DeltaTime)
{
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
    
    DrawDebugLine(GetWorld(), GetActorLocation(), LockedTarget->GetActorLocation(), FColor::Green, false, -1.0f, 0, 1.0f);
}

void ABaseFlyEnemy::AimAtTarget(const FVector& DirectionToTarget, float DeltaTime)
{
    // Gradually rotate towards the target but reduce speed to simulate aiming
    FRotator DesiredRotation = DirectionToTarget.Rotation();
    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaTime, RotationSpeed);
    SetActorRotation(NewRotation);

    // Slow down or stop movement during aiming
    FVector NewVelocity = StaticMesh->GetPhysicsLinearVelocity() * 0.95f;
    StaticMesh->SetPhysicsLinearVelocity(NewVelocity);

    if (NewVelocity.Size() < 1.0f)
    {
        StaticMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    }

    DrawDebugLine(GetWorld(), GetActorLocation(), LockedTarget->GetActorLocation(), FColor::Yellow, false, -1.0f, 0, 1.0f);
}

void ABaseFlyEnemy::ChargeTowardsPosition(const FVector& DirectionToTarget, float DeltaTime)
{
    // Calculate the desired rotation
    FRotator DesiredRotation = GetActorRotation();

    // Apply the pitch and yaw adjustments
    DesiredRotation.Yaw += ChargeYawRotationFactor * DeltaTime;
    
    // Set the adjusted rotation
    SetActorRotation(DesiredRotation);

    // Calculate the angle between the forward vector and the direction to the target
    float DotProduct = FVector::DotProduct(GetActorForwardVector(), DirectionToTarget);
    float AngleDegrees = FMath::Acos(DotProduct) * (180.0f / PI);

    // Apply force towards the target
    FVector Force = DirectionToTarget * ChargeAcceleration;
    StaticMesh->AddForce(Force, NAME_None, true);

    // Optional: Set the velocity to a fixed speed
    FVector Velocity = StaticMesh->GetPhysicsLinearVelocity().GetSafeNormal() * MovementSpeed;
    StaticMesh->SetPhysicsLinearVelocity(Velocity);

    // Optional: Apply angular damping to reduce spinning from physics
    StaticMesh->SetAngularDamping(5.0f);  // Adjust this value for the desired damping effect
    StaticMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);  // Reset angular velocity

    DrawDebugLine(GetWorld(), GetActorLocation(), LockedTarget->GetActorLocation(), FColor::Red, false, -1.0f, 0, 1.0f);
}