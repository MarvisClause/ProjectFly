// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

// Sets default values
ASpawnManager::ASpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	InitialSpawnCount = 10; // Количество платформ для спавна при запуске игры
}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	 
	bIsSpawning = false;

	// Spawn initial platform at the position of spawn manager
	LastSpawnedPlatform = GetWorld()->SpawnActor<ABaseLevelTemplate>(ABaseLevelTemplate::StaticClass(), GetActorTransform());


	// Сразу спавнит 10 экземпляров BaseLevelTemplate
	for (int32 i = 0; i < InitialSpawnCount; ++i)
	{
		SpawnPlatform();
	}
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

} 

void ASpawnManager::StartSpawning()
{
	if (BaseObjectTemplates.Num() > 0)
	{
		bIsSpawning = true;
	}
}

void ASpawnManager::StopSpawning()
{
	bIsSpawning = false;
}


void ASpawnManager::SpawnPlatform()
{
	if (BaseObjectTemplates.Num() > 0)
	{
		// Get random base object template class
		TSubclassOf<ABaseLevelTemplate> BaseLevelTemplateClass = BaseObjectTemplates[FMath::RandRange(0, BaseObjectTemplates.Num() - 1)];

		// Get spawn location, rotation for the next level template instance 
		FVector SpawnLocation = LastSpawnedPlatform->GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;
		FActorSpawnParameters SpawnParams;

		// Spawn level template
		ABaseLevelTemplate* SpawnedLevelTemplate =
			GetWorld()->SpawnActor<ABaseLevelTemplate>(BaseLevelTemplateClass, SpawnLocation, SpawnRotation, SpawnParams);

		// Check, if level was spawned
		if (IsValid(SpawnedLevelTemplate))
		{
			// Get spawned level template arrow position to calculate next position for the level template
			FVector Arrow1Location = LastSpawnedPlatform->GetArrow1()->GetComponentLocation();
			FVector Arrow2Location = LastSpawnedPlatform->GetArrow2()->GetComponentLocation();

			// Calculate the difference between the positions of Arrow1 and Arrow2
			FVector ArrowDifference = Arrow2Location - Arrow1Location;

			// Update actor position by adding ArrowDifference to the previous position
			SpawnedLevelTemplate->SetActorLocation(SpawnLocation + ArrowDifference);

			// Update pointer to the last spawned level template instance
			LastSpawnedPlatform = SpawnedLevelTemplate;
		}
	}
}

