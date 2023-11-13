// Property of (U.G.O.F, Stranget Badger, Chet kak to tak, Friends) 

#include "BoosterObject.h"
#include "Components/BoxComponent.h"
#include "ProjectFly/Pawns/BaseFlyPlane.h"

// Sets default values
ABoosterObject::ABoosterObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BaseSceneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseSceneComponent"));
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
}

void ABoosterObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Push plane upwards
	for (int32 Index = 0; Index < AffectedPlanesArray.Num(); ++Index)
	{
		// Gradually increase the influence level
		AffectedPlanesArray[Index].Value = FMath::Clamp(AffectedPlanesArray[Index].Value + (DeltaTime * BoosterSpeedIncreaseValue) * BoosterInfluenceScalar, 0.0f, 1.0f);

		// Push plane upwards with scaled impulse
		AffectedPlanesArray[Index].Key->GetStaticMesh()->AddImpulse(StaticMesh->GetUpVector() * BoosterPushScalar * AffectedPlanesArray[Index].Value);

		// Increase plane's speed with scaled value
		AffectedPlanesArray[Index].Key->AddSpeed(BoosterSpeedIncreaseValue * AffectedPlanesArray[Index].Value);

		// Rotate plane to the booster upright direction with scaled rotation
		AffectedPlanesArray[Index].Key->GetStaticMesh()->SetRelativeRotation(
			FMath::Lerp(AffectedPlanesArray[Index].Key->GetStaticMesh()->GetRelativeRotation(), StaticMesh->GetUpVector().ToOrientationRotator(), DeltaTime * AffectedPlanesArray[Index].Value)
		);
	}
}

void ABoosterObject::OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

	PrimaryActorTick.SetTickFunctionEnable(AffectedPlanesArray.Num() != 0);
}

void ABoosterObject::OnTriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

	PrimaryActorTick.SetTickFunctionEnable(AffectedPlanesArray.Num() != 0);
}
