#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GliderPlayerController.generated.h"

class AGliderPawn;
class UGliderHUDWidget;
class UInputMappingContext;
class UInputAction;
class UGlobalInputDataConfig;
class UGliderInputDataConfig;

UCLASS()
class PROJECTFLY_API AGliderPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    virtual void SetupInputComponent() override;

protected:
    void EnableGameplayInput();
    void DisableGameplayInput();

    virtual void TogglePause();

    UPROPERTY()
    TObjectPtr<AGliderPawn> GliderPawn;

    UPROPERTY()
    TObjectPtr<UGliderHUDWidget> HUDWidget;

private:
    void OnLook(const FInputActionValue& Value);
    void OnPitch(const FInputActionValue& Value);
    void OnYaw(const FInputActionValue& Value);
    void OnRoll(const FInputActionValue& Value);

    void OnDashStarted(const FInputActionValue&);
    void OnDashReleased(const FInputActionValue&);

    void OnHaltStarted(const FInputActionValue&);
    void OnHaltReleased(const FInputActionValue&);

    void OnFreeLookStarted(const FInputActionValue&);
    void OnFreeLookReleased(const FInputActionValue&);

    UFUNCTION()
    void HandleHintTriggered(FText Message, float DisplayTime);

    UPROPERTY(EditDefaultsOnly, Category="Enhanced Input")
    UInputMappingContext* GliderContext;

    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputMappingContext* GlobalContext;

    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UGliderInputDataConfig* GliderActions;

    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UGlobalInputDataConfig* GlobalActions;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGliderHUDWidget> HUDClass;
};