#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimedFlyGameMode.generated.h"

UENUM(BlueprintType)
enum class EFlyResult : uint8
{
    Finished,
    Failed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFlyEnded, EFlyResult, FlyResult, float, TimeInSeconds);

UCLASS()
class PROJECTFLY_API ATimedFlyGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATimedFlyGameMode();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    // Timer Control
    void StopTime();
    void ResumeTime();

    float GetCurrentTimeInSeconds() const
    {
        return CurrentTimeInSeconds;
    }

    // Fires, when players reaches trigger or dies
    UPROPERTY(BlueprintAssignable)
    FOnFlyEnded OnFlyEnded;

private:
    UFUNCTION()
    void HandleFinishTriggerReached();

    UFUNCTION()
    void HandlePlayerDeath();

    // Tracks time
    float CurrentTimeInSeconds = 0.f;

    // Defines, if timer should work
    bool bTimerRunning;

    // Defines, if level is finished
    bool bIsFinished = false;
};