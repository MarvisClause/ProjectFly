// Property of (U.G.O.F, Stranget Badger, Chet kak to tak, Friends) 

#include "DeathWall.h"
#include "Components/BoxComponent.h"
#include "ProjectFly/Pawns/BaseFlyPlane.h"
#include "EngineUtils.h"

// Sets default values
ADeathWall::ADeathWall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	BaseSceneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseSceneComponent"));
	SetRootComponent(BaseSceneComponent);

	SpeedDecreaseTriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpeedDecreaseTriggerArea"));
	SpeedDecreaseTriggerArea->SetupAttachment(BaseSceneComponent);
}

void ADeathWall::BeginPlay()
{
	Super::BeginPlay();

	// Find plane in the level
	// This is temporary solution. I think it is better to handle this through game mode.
	
	// Get the world
	UWorld* World = GetWorld();
	if (!World) 
	{
		return;
	}

	// Iterate over all actors in the world
	for (TActorIterator<ABaseFlyPlane> ActorItr(World); ActorItr; ++ActorItr)
	{
		ABaseFlyPlane* FoundPlane = *ActorItr;

		if (IsValid(FoundPlane))
		{
			TargetPlane = FoundPlane;
			break; 
		}
	}

	SpeedDecreaseTriggerArea->OnComponentBeginOverlap.AddDynamic(this, &ADeathWall::OnTriggerAreaBeginOverlap);
	SpeedDecreaseTriggerArea->OnComponentEndOverlap.AddDynamic(this, &ADeathWall::OnTriggerAreaEndOverlap);
}

void ADeathWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Follow and rotate towards target plane
	if (IsValid(TargetPlane))
	{
		// Calculate direction from ADeathWall to the target plane
		FVector Direction = TargetPlane->GetActorLocation() - GetActorLocation();

		// Calculate the rotation around the Z-axis
		FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		FRotator DeltaRotation = NewRotation - GetActorRotation();
		DeltaRotation.Pitch = 0;
		DeltaRotation.Roll = 0;

		// Adjust the rotation speed
		DeltaRotation = FMath::Lerp(FRotator::ZeroRotator, DeltaRotation, RotationSpeedValue * DeltaTime);

		// Rotate ADeathWall towards the target plane around the global Z-axis
		SetActorRotation(GetActorRotation() + DeltaRotation);

		// Move the ADeathWall towards the target plane
		FVector TargetLocation = FMath::VInterpTo(GetActorLocation(), TargetPlane->GetActorLocation(), DeltaTime, ChaseSpeedValue);
		SetActorLocation(TargetLocation);
	}

	// Decrease plane speed
	for (int32 Index = 0; Index < AffectedPlanesArray.Num(); ++Index)
	{
		// Gradually increase the influence level
		AffectedPlanesArray[Index].Value = FMath::Clamp(AffectedPlanesArray[Index].Value + (DeltaTime * SpeedDecreaseValue) * SpeedDecreaseValue, 0.0f, 1.0f);

		// Increase plane's speed with scaled value
		AffectedPlanesArray[Index].Key->AddSpeed(-(SpeedDecreaseValue * AffectedPlanesArray[Index].Value));
	}
}

void ADeathWall::OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseFlyPlane* FlyPlane = Cast<ABaseFlyPlane>(OtherActor))
	{
		for (int32 Index = 0; Index < AffectedPlanesArray.Num(); ++Index)
		{
			if (AffectedPlanesArray[Index].Key == FlyPlane)
			{
				AffectedPlanesArray.RemoveAt(Index);
			}
		}
		AffectedPlanesArray.Add(TTuple<TObjectPtr<ABaseFlyPlane>, float>(FlyPlane, 0.0f));
	}
}

void ADeathWall::OnTriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseFlyPlane* FlyPlane = Cast<ABaseFlyPlane>(OtherActor))
	{
		for (int32 Index = 0; Index < AffectedPlanesArray.Num(); ++Index)
		{
			if (AffectedPlanesArray[Index].Key == FlyPlane)
			{
				AffectedPlanesArray.RemoveAt(Index);
			}
		}
	}
}
