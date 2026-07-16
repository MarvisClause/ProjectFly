#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "GliderInputDataConfig.generated.h"

UCLASS(BlueprintType)
class PROJECTFLY_API UGliderInputDataConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* PitchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* YawAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* RollAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* HaltAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* FreeLookAction;
};