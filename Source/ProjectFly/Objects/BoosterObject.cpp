#include "BoosterObject.h"
#include "Components/BoxComponent.h"
#include "ProjectFly/Pawns/GliderPawn.h"

// Sets default values
ABoosterObject::ABoosterObject()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    SetRootComponent(RootSceneComponent);

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

    HandleActiveGliders(DeltaTime);

    HandleExitingGliders(DeltaTime);

    PrimaryActorTick.SetTickFunctionEnable(AffectedGlidersArray.Num() > 0 || ExitingGlidersArray.Num() > 0);
}

void ABoosterObject::OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (AGliderPawn* Glider = Cast<AGliderPawn>(OtherActor))
    {
        // Remove existing entry if any
        AffectedGlidersArray.RemoveAll([Glider](const TTuple<TObjectPtr<AGliderPawn>, float>& Elem) { return Elem.Key == Glider; });
        ExitingGlidersArray.RemoveAll([Glider](const TTuple<TObjectPtr<AGliderPawn>, float>& Elem) { return Elem.Key == Glider; });

        // Add with zero influence
        AffectedGlidersArray.Add(TTuple<TObjectPtr<AGliderPawn>, float>(Glider, 0.f));

        // Start removing camera lag
        Glider->StartRemovingCameraLag();
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

        // Start enabling camera lag
        Glider->StartEnablingCameraLag();
    }

    PrimaryActorTick.SetTickFunctionEnable(AffectedGlidersArray.Num() > 0 || ExitingGlidersArray.Num() > 0);
}

float ABoosterObject::CalculateDistanceFactor(AGliderPawn* Glider) const
{
    const FVector BoosterCenter = StaticMesh->GetComponentLocation();
    const float Distance = FVector::Dist(BoosterCenter, Glider->GetActorLocation());
    const float MaxDistance = BoosterTriggerArea->GetScaledBoxExtent().Size();
    return FMath::Clamp(1.0f - (Distance / MaxDistance), 0.1f, 1.0f);
}

float ABoosterObject::ComputeFinalInfluence(float BaseInfluence, float DistanceFactor) const
{
    return BaseInfluence * (bDistanceBasedBoosterPush ? DistanceFactor : 1.0f);
}

void ABoosterObject::ApplyBoosterEffect(AGliderPawn* Glider, float Influence, float DeltaTime, bool bAlignToBooster)
{
    if (!Glider) return;

    UStaticMeshComponent* Mesh = Glider->GetStaticMesh();
    if (!Mesh) return;

    // Impulse and speed increase
    Mesh->AddImpulse(StaticMesh->GetUpVector() * BoosterPushScalar * Influence);
    Glider->AffectSpeed(BoosterSpeedIncreaseValue * Influence);

    // Orientation control
    FVector CurrentDir = Mesh->GetForwardVector();
    FVector TargetDir = bAlignToBooster ? StaticMesh->GetUpVector() : CurrentDir;

    // Calculate angular difference between directions
    float AngleError = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CurrentDir, TargetDir)));
    FVector RotationAxis = FVector::CrossProduct(CurrentDir, TargetDir).GetSafeNormal();

    // Scale torque depending on how large the error is
    float AlignmentStrength = FMath::Clamp(AngleError / 45.0f, 0.0f, 1.0f);

    // Damping: reduce rotation if we're close to target
    float TorqueStrength = BoosterRotationScalar * Influence * AlignmentStrength;

    // Apply torque gradually to rotate towards target
    FVector Torque = RotationAxis * TorqueStrength;

    Mesh->AddTorqueInRadians(Torque, NAME_None, true);
}

void ABoosterObject::HandleActiveGliders(float DeltaTime)
{
    // Iterate over active gliders
    for (int32 i = 0; i < AffectedGlidersArray.Num(); ++i)
    {
        auto& Pair = AffectedGlidersArray[i];
        AGliderPawn* Glider = Pair.Key.Get();
        if (!Glider)
        {
            AffectedGlidersArray.RemoveAt(i);
            continue;
        }

        // Define influence based on distance and incremental factor, if one is enabled 
        const float DistanceFactor = CalculateDistanceFactor(Glider);
        Pair.Value = bIncrementalEnterBoosterPush ? FMath::Clamp(Pair.Value + DeltaTime, 0.f, 1.f) : 1.f;
        const float FinalInfluence = ComputeFinalInfluence(Pair.Value, DistanceFactor);

        ApplyBoosterEffect(Glider, FinalInfluence, DeltaTime, true);
    }
}

void ABoosterObject::HandleExitingGliders(float DeltaTime)
{
    // Iterate over exiting gliders
    for (int32 i = 0; i < ExitingGlidersArray.Num(); ++i)
    {
        auto& Pair = ExitingGlidersArray[i];
        AGliderPawn* Glider = Pair.Key.Get();
        if (!Glider)
        {
            ExitingGlidersArray.RemoveAt(i);
            continue;
        }

        // Affect glider for some time before diminishing effect on it
        Pair.Value += DeltaTime;
        const float Alpha = Pair.Value / BoosterFalloffDuration;

        // Remove glider once fallof duration is up
        if (Alpha >= 1.0f)
        {
            ExitingGlidersArray.RemoveAt(i);
            continue;
        }

        // Calculate diminishing falloff influence
        const float FalloffInfluence = FMath::Pow(1.0f - Alpha, 3.0f);
        const float DistanceFactor = CalculateDistanceFactor(Glider);
        const float FinalInfluence = ComputeFinalInfluence(FalloffInfluence, DistanceFactor);

        ApplyBoosterEffect(Glider, FinalInfluence, DeltaTime, false);
    }
}