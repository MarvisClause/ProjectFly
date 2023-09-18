// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "BaseLevelTemplate.generated.h"

UCLASS()
class PROJECTFLY_API ABaseLevelTemplate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseLevelTemplate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		UArrowComponent* GetArrow1() const { return Arrow1; }

	UFUNCTION(BlueprintCallable)
		UArrowComponent* GetArrow2() const { return Arrow2; }

private:
	// Бокс-коллайдер
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxCollider;

	// Стрелка 1
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* Arrow1;

	// Стрелка 2
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* Arrow2;

	// Обработчик пересечения с бокс-коллайдером
	UFUNCTION()
		void OnBoxColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Обработчик задержки
	FTimerHandle DelayTimerHandle;

	// Функция для удаления платформы с задержкой
	void DestroyPlatformWithDelay();
};
