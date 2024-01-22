#pragma once

#include "ProjectFly/LevelManager/Parts/BaseLevelPart.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ConnectorLevelPart.generated.h"

UCLASS()
class PROJECTFLY_API AConnectorLevelPart : public ABaseLevelPart
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AConnectorLevelPart();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

protected:
    // Box trigger component that detects when the gateway is entered
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Connector")
    UBoxComponent* GatewayTrigger;

    // Visual block mesh for the entrance to the gateway
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Connector")
    UStaticMeshComponent* EnterGatewayBlock;

    // Visual block mesh for the exit from the gateway
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Connector")
    UStaticMeshComponent* ExitGatewayBlock;

    // Event triggered when the gateway is entered
    UFUNCTION(BlueprintImplementableEvent, Category = "Connector")
    void OnGatewayTriggerEnter(AActor* OtherActor);

private:
    // Function to handle logic when something enters the gateway
    UFUNCTION()
    void HandleGatewayEnter(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Function to close the gateway after a delay
    UFUNCTION()
    void CloseGateway();

    // Function to notify upper managers about entering the connector
    UFUNCTION()
    void NotifyUpperManagers();

    // Delay before closing the gateway (in seconds)
    UPROPERTY(EditDefaultsOnly, Category = "Connector")
    float CloseDelay;

    // Flag to check if the gateway is open
    bool bIsGatewayOpen;
};