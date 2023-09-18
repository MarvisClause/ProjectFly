// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectFly/Objects/BaseLevelTemplate.h"
#include "SpawnManager.generated.h"

UCLASS()
class PROJECTFLY_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void StartSpawning();

	UFUNCTION(BlueprintCallable)
		void StopSpawning();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnManager")
		TArray<TSubclassOf<ABaseLevelTemplate>> BaseObjectTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnManager")
		float SpawnInterval;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnManager")
		int32 InitialSpawnCount; // Количество платформ для спавна при запуске игры  

	void SpawnPlatform();

private:
	bool bIsSpawning;
	FTimerHandle SpawnTimer;

	UPROPERTY()
		ABaseLevelTemplate* LastSpawnedPlatform;
};
