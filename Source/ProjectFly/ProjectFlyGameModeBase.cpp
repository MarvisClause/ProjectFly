// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProjectFlyGameModeBase.h"
#include "ProjectFly/LevelManager/LevelManager.h"

void AProjectFlyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Initialize level manager
	LevelManager = GetWorld()->SpawnActor<ALevelManager>(LevelManagerClass);

	// Start generation
	LevelManager->SpawnNextBiome();
}
