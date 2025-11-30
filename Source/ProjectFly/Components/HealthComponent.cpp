#include "ProjectFly/Components/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void UHealthComponent::ApplyDamage(float Amount)
{
	if (Amount <= 0.f || CurrentHealth <= 0.f)
		return;

	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.f, MaxHealth);

	// Broadcast health update
	OnHealthChanged.Broadcast(CurrentHealth, Amount);

	// Death check
	if (CurrentHealth <= 0.f)
	{
		OnDeath.Broadcast();
	}
}
