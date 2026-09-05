#include "HintMessageTrigger.h"

#include "Components/BoxComponent.h"
#include "ProjectFly/Pawns/GliderPawn.h"

AHintMessageTrigger::AHintMessageTrigger()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    HintTriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("HintTriggerArea"));
    RootComponent = HintTriggerArea;
}

void AHintMessageTrigger::BeginPlay()
{
    Super::BeginPlay();

    HintTriggerArea->OnComponentBeginOverlap.AddDynamic(this, &AHintMessageTrigger::OnHintTriggerAreaBeginOverlap);
}

void AHintMessageTrigger::OnHintTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if ( !Cast<AGliderPawn>(OtherActor) )
    {
        return;
    }

    if (bTriggerOnce)
    {
        HintTriggerArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    OnHintTriggered.Broadcast(HintMessage, DisplayTime);
}