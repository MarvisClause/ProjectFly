#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "GlobalInputDataConfig.generated.h"

UCLASS(BlueprintType)
class PROJECTFLY_API UGlobalInputDataConfig : public UDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly)
    UInputAction* PauseAction;
};