#pragma once

#include "ProjectFly/LevelManager/Parts/BaseLevelPart.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ConnectorLevelPart.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGatewayEnter, AConnectorLevelPart*, ConnectorLevelPart);

UCLASS(Abstract)
class PROJECTFLY_API AConnectorLevelPart : public ABaseLevelPart
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AConnectorLevelPart();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Gateway enter delegate
    FGatewayEnter OnGatewayEnterDelegate;

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

private:
    // Function to handle logic when something enters the gateway
    UFUNCTION()
    void HandleGatewayEnter(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Function to open the gateway
    void OpenGateway();

    // Function to close the gateway after a delay
    UFUNCTION()
    void CloseGateway();

    // Function to notify upper managers about entering the connector
    UFUNCTION()
    void NotifyUpperManagers();

    // Delay before closing the gateway (in seconds)
    UPROPERTY(EditDefaultsOnly, Category = "Connector")
    float CloseDelay;

    // Flag to check if the gateway is open. It is used in case of player turning around in the connector, so it won't engage gateway trigger again
    bool bIsGatewayOpen;
};