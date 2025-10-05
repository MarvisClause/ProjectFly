#include "BoosterObject.h"
#include "Components/BoxComponent.h"
#include "ProjectFly/Pawns/GliderPawn.h"

// Sets default values
ABoosterObject::ABoosterObject()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    BaseSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BaseSceneComponent"));
    SetRootComponent(BaseSceneComponent);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(RootComponent);

    BoosterTriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoosterTriggerArea"));
    BoosterTriggerArea->SetupAttachment(StaticMesh);
}

void ABoosterObject::BeginPlay()
{
    Super::BeginPlay();

    BoosterTriggerArea->OnComponentBeginOverlap.AddDynamic(this, &ABoosterObject::OnTriggerAreaBeginOverlap);
    BoosterTriggerArea->OnComponentEndOverlap.AddDynamic(this, &ABoosterObject::OnTriggerAreaEndOverlap);

    // Delay the overlap check by 1 second
    FTimerHandle CheckHandle;
    GetWorldTimerManager().SetTimer(
        CheckHandle,
        [this]()
        {
            // Manually check for gliders already inside
            TArray<AActor*> OverlappingActors;
            BoosterTriggerArea->GetOverlappingActors(OverlappingActors, AGliderPawn::StaticClass());

            for (AActor* Actor : OverlappingActors)
            {
                if (AGliderPawn* Glider = Cast<AGliderPawn>(Actor))
                {
                    // Simulate an overlap event
                    OnTriggerAreaBeginOverlap(BoosterTriggerArea, Glider, nullptr, 0, false, FHitResult());
                }
            }
        },
        1.0f,   // Delay in seconds
        false   // bLoop = false
    );
}

void ABoosterObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Active gliders in the booster trigger handling
    for (int32 Index = 0; Index < AffectedGlidersArray.Num(); ++Index)
    {
        auto& Pair = AffectedGlidersArray[Index];
        AGliderPawn* Glider = Pair.Key.Get();

        if (!Glider)
        {
            AffectedGlidersArray.RemoveAt(Index);
            --Index;
            continue;
        }

        // Calculate distance-based falloff
        const FVector BoosterCenter = StaticMesh->GetComponentLocation();
        const FVector GliderLocation = Glider->GetActorLocation();
        const float Distance = FVector::Dist(BoosterCenter, GliderLocation);

        // Estimate max radius from trigger size
        const FVector BoxExtent = BoosterTriggerArea->GetScaledBoxExtent();
        const float MaxDistance = BoxExtent.Size(); // diagonal distance
        const float DistanceFactor = FMath::Clamp(1.0f - (Distance / MaxDistance), 0.1f, 1.0f);

        // Combine time-based buildup + distance falloff
        Pair.Value = bIncrementalEnterBoosterPush ? FMath::Clamp(Pair.Value + DeltaTime, 0.f, 1.f) : 1.0f;
        const float FinalInfluence = Pair.Value * (bDistanceBasedBoosterPush? DistanceFactor : 1.0f);

        // Apply impulse on glider's mesh in booster up direction scaled by BoosterPushScalar and influence
        Glider->GetStaticMesh()->AddImpulse(StaticMesh->GetUpVector() * BoosterPushScalar * FinalInfluence);

        // Increase forward speed by scaled booster speed increase
        Glider->AffectSpeed(BoosterSpeedIncreaseValue * FinalInfluence);

        // Smoothly rotate glider toward booster up vector
        FRotator CurrentRot = Glider->GetStaticMesh()->GetComponentRotation();
        FRotator TargetRot = StaticMesh->GetUpVector().ToOrientationRotator();
        FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, BoosterRotationScalar * FinalInfluence);
        Glider->GetStaticMesh()->SetWorldRotation(NewRot);
    }

    // Exiting gliders out of the booster trigger handling
    for (int32 Index = 0; Index < ExitingGlidersArray.Num(); ++Index)
    {
        auto& Pair = ExitingGlidersArray[Index];
        AGliderPawn* Glider = Pair.Key.Get();
        if (!Glider)
        {
            ExitingGlidersArray.RemoveAt(Index--);
            continue;
        }

        Pair.Value += DeltaTime;
        const float Alpha = Pair.Value / BoosterFalloffDuration;

        if (Alpha >= 1.0f)
        {
            ExitingGlidersArray.RemoveAt(Index--);
            continue;
        }

        // Smooth (nonlinear) falloff: cubic ease-out for smooth end
        const float FalloffInfluence = FMath::Pow(1.0f - Alpha, 3.0f);

        // Distance-based scaling (optional)
        const FVector BoosterCenter = StaticMesh->GetComponentLocation();
        const FVector GliderLocation = Glider->GetActorLocation();
        const float Distance = FVector::Dist(BoosterCenter, GliderLocation);
        const float MaxDistance = BoosterTriggerArea->GetScaledBoxExtent().Size();
        const float DistanceFactor = FMath::Clamp(1.0f - (Distance / MaxDistance), 0.1f, 1.0f);

        // Apply force and speed falloff
        const float FinalInfluence = FalloffInfluence * (bDistanceBasedBoosterPush ? DistanceFactor : 1.0f);

        Glider->GetStaticMesh()->AddImpulse(StaticMesh->GetUpVector() * BoosterPushScalar * FinalInfluence);
        Glider->AffectSpeed(BoosterSpeedIncreaseValue * FinalInfluence);

        // Smoothly rotate glider back to its natural orientation or world up
        FRotator CurrentRot = Glider->GetStaticMesh()->GetComponentRotation();
        FRotator TargetRot = FRotator(0.f, CurrentRot.Yaw, 0.f); // or Glider->GetActorRotation() if it has a base rotation
        FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, BoosterRotationScalar * FinalInfluence);
        Glider->GetStaticMesh()->SetWorldRotation(NewRot);
    }

    PrimaryActorTick.SetTickFunctionEnable(AffectedGlidersArray.Num() > 0 || ExitingGlidersArray.Num() > 0);
}

void ABoosterObject::OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (AGliderPawn* Glider = Cast<AGliderPawn>(OtherActor))
    {
        // Remove existing entry if any
        AffectedGlidersArray.RemoveAll([Glider](const TTuple<TObjectPtr<AGliderPawn>, float>& Elem) { return Elem.Key == Glider; });

        // Add with zero influence
        AffectedGlidersArray.Add(TTuple<TObjectPtr<AGliderPawn>, float>(Glider, 0.f));
    }

    PrimaryActorTick.SetTickFunctionEnable(AffectedGlidersArray.Num() > 0 || ExitingGlidersArray.Num() > 0);
}

void ABoosterObject::OnTriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (AGliderPawn* Glider = Cast<AGliderPawn>(OtherActor))
    {
        // Remove from active boosters
        const int32 Removed = AffectedGlidersArray.RemoveAll(
            [Glider](const TTuple<TObjectPtr<AGliderPawn>, float>& Elem) { return Elem.Key == Glider; });

        if (Removed > 0 && bIncrementalExitBoosterPush)
        {
            // Add to exiting list (start falloff timer)
            ExitingGlidersArray.Add(TTuple<TObjectPtr<AGliderPawn>, float>(Glider, 0.0f));
        }
    }

    PrimaryActorTick.SetTickFunctionEnable(AffectedGlidersArray.Num() > 0 || ExitingGlidersArray.Num() > 0);
}
