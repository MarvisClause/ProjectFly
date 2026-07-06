#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FinishTrigger.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishReached);

UCLASS()
class PROJECTFLY_API AFinishTrigger : public AActor
{
    GENERATED_BODY()

public:
    AFinishTrigger();

    UPROPERTY(BlueprintAssignable)
    FOnFinishReached OnFinishReached;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void OnTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Scene component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> RootSceneComponent;

    // Finish trigger area
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBoxComponent> FinishTriggerArea;
};