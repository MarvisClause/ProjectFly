
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseFlyPlane.generated.h"

class USpringArmComponent;
class UCameraComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBaseFlyPlane, Log, All);

// Basic pawn, which imitates paper plane
UCLASS(Abstract)
class PROJECTFLY_API ABaseFlyPlane : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ABaseFlyPlane();

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Adds speed to the plane
    void AddSpeed(float Speed);
    // Returns plane speed
    float GetSpeed() const;

    UStaticMeshComponent* GetStaticMesh() const;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    #pragma region PlaneControlSettings

    // Plane control settings

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Control", meta = (ClampMin = 0.0f))
    float DiveSpeedIncreaseScalar = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Control", meta = (ClampMin = 0.0f))
    float RiseSpeedDecreaseScalar = 4.0f;

    float AirControl = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Control", meta = (ClampMin = 0.0f))
    float MinimumAirControl = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Control", meta = (ClampMin = 0.0f))
    float MaximumAirControl = 9.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Control", meta = (ClampMin = 0.0f))
    float TurbulenceFactor = 8.0f;

    // Negative value allows plane to move backwards, thus imitating reversed gliding
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Control", meta = (ClampMin = 0.0f))
    float MinimumPlaneSpeed = -100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Control", meta = (ClampMin = 0.0f))
    float MaximumPlaneSpeed = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Control", meta = (ClampMin = 0.0f))
    float StartPlaneSpeed = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Control", meta = (ClampMin = 0.0f))
    float AutoRotationPlaneSpeedScalar = 2.0f;

    // Defines speed threshold, under which plane will start to dive downwards automatically
    const float PlaneSpeedThresholdForPitchDecline = 300.0f;

    #pragma endregion

    #pragma region CameraControlSettings

    // Camera control settings

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Camera Control", meta = (ClampMin = 0.0f))
    float MinimumCameraBoomLength = 250.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane Camera Control", meta = (ClampMin = 0.0f))
    float MaximumCameraBoomLength = 600.0f;

    #pragma endregion

private:

    // Input process
    void PitchControl(float Value);
    void YawControl(float Value);
    void RollControl(float Value);

    // Calculates change of speed from inclination 
    void CalculateSpeed(float DeltaTime);
    // Calculates rotation changes
    void CalculateRotation(float DeltaTime);
    // Updates camera
    void UpdateCamera();

    UFUNCTION()
    void OnPlaneHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* StaticMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* FocusSceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    // Forward speed of the plane
    // This is the main variable, which defines speed of the plane 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float ForwardSpeed = 0.0f;
};
