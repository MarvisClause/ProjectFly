// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectFlyGameModeBase.generated.h"

class ALevelManager;

/**
 * Controls main game flow 
 */
UCLASS(Abstract)
class PROJECTFLY_API AProjectFlyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ALevelManager> LevelManagerClass;

	UPROPERTY()
	ALevelManager* LevelManager;
};
