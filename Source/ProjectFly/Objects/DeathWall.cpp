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

void ADeathWall::SetMoveSpeed(float MoveSpeed)
{
	MoveSpeedValue = MoveSpeed;
}

void ADeathWall::AddMovePoint(FVector TargetPosition, FRotator TargetRotation, FVector NewDeathWallBoxExtent)
{
	DeathWallMovePoint MovePoint;
	MovePoint.TargetPosition = TargetPosition;
	MovePoint.TargetRotation = TargetRotation;
	MovePoint.NewDeathWallBoxExtent = NewDeathWallBoxExtent;

	DeathWallMovePointArray.Add(MovePoint);
}

void ADeathWall::ClearAllMovePoints()
{
	DeathWallMovePointArray.Empty();
}

void ADeathWall::BeginPlay()
{
	Super::BeginPlay();

	SpeedDecreaseTriggerArea->OnComponentBeginOverlap.AddDynamic(this, &ADeathWall::OnTriggerAreaBeginOverlap);
	SpeedDecreaseTriggerArea->OnComponentEndOverlap.AddDynamic(this, &ADeathWall::OnTriggerAreaEndOverlap);
}

void ADeathWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DeathWallMovePointArray.Num() > 0)
	{
		// Check, if death wall was resized to fit to the given level part
		if (DeathWallMovePointArray[0].bWasResized == false)
		{
			// Reset scale factor
			SpeedDecreaseTriggerArea->SetWorldScale3D(FVector(1.0, 1.0, 1.0));

			// Get the size of the level part mesh
			FVector LevelPartSize = DeathWallMovePointArray[0].NewDeathWallBoxExtent * 2.0f;

			// Get the size of the death wall mesh
			FVector DeathWallSize = SpeedDecreaseTriggerArea->Bounds.BoxExtent * 2.0f;

			// Calculate the scale factor for each axis
			FVector ScaleFactor = FVector(LevelPartSize.X / DeathWallSize.X, LevelPartSize.Y / DeathWallSize.Y, LevelPartSize.Z / DeathWallSize.Z);
			ScaleFactor.X = 50.0f;

			// Set the scale of the death wall mesh to match the size of the level part mesh
			SpeedDecreaseTriggerArea->SetWorldScale3D(ScaleFactor);

			// Update location of speed decrease trigger area to be above scene component
			SpeedDecreaseTriggerArea->SetRelativeLocation(FVector(0.0, 0.0, DeathWallMovePointArray[0].NewDeathWallBoxExtent.Z / 2));

			// Update resize marker
			DeathWallMovePointArray[0].bWasResized = true;
		}

		// Calculate direction from ADeathWall to the first move point
		FVector Direction = DeathWallMovePointArray[0].TargetPosition - GetActorLocation();

		// Calculate the rotation around the Z-axis
		FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		FRotator DeltaRotation = DeathWallMovePointArray[0].TargetRotation - GetActorRotation();
		DeltaRotation.Pitch = 0;
		DeltaRotation.Roll = 0;

		// Adjust the rotation speed
		DeltaRotation = FMath::Lerp(FRotator::ZeroRotator, DeltaRotation, RotationSpeedValue * DeltaTime);

		// Rotate ADeathWall towards the target plane around the global Z-axis
		SetActorRotation(GetActorRotation() + DeltaRotation);

		// Move the ADeathWall towards the target plane
		FVector TargetLocation = FMath::VInterpConstantTo(GetActorLocation(), DeathWallMovePointArray[0].TargetPosition, DeltaTime, MoveSpeedValue);
		SetActorLocation(TargetLocation);

		// Check, if move point was reached
		if (FVector::Dist(GetActorLocation(), DeathWallMovePointArray[0].TargetPosition) < 10.0f)
		{
			DeathWallMovePointArray.RemoveAt(0);
		}
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
