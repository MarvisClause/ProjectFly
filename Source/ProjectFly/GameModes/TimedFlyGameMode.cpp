#include "TimedFlyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectFly/Objects/FinishTrigger.h"
#include "ProjectFly/Pawns/GliderPawn.h"
#include "ProjectFly/Components/HealthComponent.h"

ATimedFlyGameMode::ATimedFlyGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATimedFlyGameMode::BeginPlay()
{
    Super::BeginPlay();

    bTimerRunning = true;

    // Find all finish triggers
    {
        TArray<AActor*> Actors;

        UGameplayStatics::GetAllActorsOfClass(
            this,
            AFinishTrigger::StaticClass(),
            Actors);

        for (AActor* Actor : Actors)
        {
            if (AFinishTrigger* Trigger = Cast<AFinishTrigger>(Actor))
            {
                Trigger->OnFinishReached.AddDynamic(this, &ATimedFlyGameMode::HandleFinishTriggerReached);
            }
        }
    }

    // Find glider pawn
    {
        TArray<AActor*> Actors;

        UGameplayStatics::GetAllActorsOfClass(
            this,
            AGliderPawn::StaticClass(),
            Actors);

        for (AActor* Actor : Actors)
        {
            if (AGliderPawn* GliderPawn = Cast<AGliderPawn>(Actor))
            {
                GliderPawn->AccessHealthComponent()->OnDeath.AddDynamic(this, &ATimedFlyGameMode::HandlePlayerDeath);
            }
        }
    }
}

void ATimedFlyGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bTimerRunning)
    {
        CurrentTimeInSeconds += DeltaSeconds;
    }
}

void ATimedFlyGameMode::StopTime()
{
    bTimerRunning = false;
}

void ATimedFlyGameMode::ResumeTime()
{
    bTimerRunning = true;
}

void ATimedFlyGameMode::HandleFinishTriggerReached()
{
    StopTime();
    OnFlyEnded.Broadcast(EFlyResult::Finished, CurrentTimeInSeconds);
}

void ATimedFlyGameMode::HandlePlayerDeath()
{
    StopTime();
    OnFlyEnded.Broadcast(EFlyResult::Failed, CurrentTimeInSeconds);
}
