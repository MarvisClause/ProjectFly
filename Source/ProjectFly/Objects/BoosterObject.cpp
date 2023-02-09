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

	// Create a static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	BoosterTriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoosterTriggerArea"));
	BoosterTriggerArea->SetupAttachment(RootComponent);
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

	for (int32 Index = 0; Index < AffectedPlanesArray.Num(); ++Index)
	{
		AffectedPlanesArray[Index]->GetStaticMesh()->AddImpulse(StaticMesh->GetUpVector() * BoosterPowerScalar);
		AffectedPlanesArray[Index]->GetStaticMesh()->AddImpulse(AffectedPlanesArray[Index]->GetStaticMesh()->GetForwardVector() * BoosterPowerScalar);
	}
}

void ABoosterObject::OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseFlyPlane* FlyPlane = Cast<ABaseFlyPlane>(OtherActor))
	{
		if (AffectedPlanesArray.Contains(FlyPlane))
		{
			AffectedPlanesArray.Remove(FlyPlane);
		}
		AffectedPlanesArray.Add(FlyPlane);
	}

	PrimaryActorTick.SetTickFunctionEnable(AffectedPlanesArray.Num() != 0);
}

void ABoosterObject::OnTriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseFlyPlane* FlyPlane = Cast<ABaseFlyPlane>(OtherActor))
	{
		if (AffectedPlanesArray.Contains(FlyPlane))
		{
			AffectedPlanesArray.Remove(FlyPlane);
		}
	}

	PrimaryActorTick.SetTickFunctionEnable(AffectedPlanesArray.Num() != 0);
}
