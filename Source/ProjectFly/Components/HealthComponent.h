#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, DamageTaken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

// Health component for handling damage
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTFLY_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	// Fires on health change
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	// Fires when health reaches zero
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	// Apply damage
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float Amount);

protected:
	virtual void BeginPlay() override;
};