#include "FinishTrigger.h"
#include "Components/BoxComponent.h"
#include "ProjectFly/Pawns/GliderPawn.h"
#include "ProjectFly/GameModes/TimedFlyGameMode.h"

// Sets default values
AFinishTrigger::AFinishTrigger()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    SetRootComponent(RootSceneComponent);

    FinishTriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("FinishTriggerArea"));
    FinishTriggerArea->SetupAttachment(RootSceneComponent);
}

void AFinishTrigger::BeginPlay()
{
    Super::BeginPlay();

    FinishTriggerArea->OnComponentBeginOverlap.AddDynamic(this, &AFinishTrigger::OnTriggerAreaBeginOverlap);
}

void AFinishTrigger::OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (Cast<AGliderPawn>(OtherActor))
    {
        OnFinishReached.Broadcast();
    }
}