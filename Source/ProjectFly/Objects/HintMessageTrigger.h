#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HintMessageTrigger.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHintTriggered, FText, Message, float, DisplayTime);

UCLASS()
class PROJECTFLY_API AHintMessageTrigger : public AActor
{
	GENERATED_BODY()

public:
    AHintMessageTrigger();

    UPROPERTY(BlueprintAssignable)
    FOnHintTriggered OnHintTriggered;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHintTriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBoxComponent> HintTriggerArea;

    UPROPERTY(EditInstanceOnly, Category = "Hint")
    FText HintMessage;

    UPROPERTY(EditInstanceOnly, Category = "Hint")
    float DisplayTime = 5.0f;

    UPROPERTY(EditInstanceOnly, Category = "Hint")
    bool bTriggerOnce = true;
};