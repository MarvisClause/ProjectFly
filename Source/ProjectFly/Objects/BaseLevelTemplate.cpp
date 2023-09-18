// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseLevelTemplate.h"
#include "ProjectFly/Managers/SpawnManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h" // Для GetWorld()
#include "GameFramework/Actor.h" // Для Destroy()

// Sets default values
ABaseLevelTemplate::ABaseLevelTemplate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Создание корневого компонента
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Создание и настройка бокс-коллайдера
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(RootComponent);

	// Создание и настройка первой стрелки
	Arrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow1"));
	Arrow1->SetupAttachment(RootComponent);

	// Создание и настройка второй стрелки
	Arrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow2"));
	Arrow2->SetupAttachment(RootComponent);

	//mmm danonn
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseLevelTemplate::OnBoxColliderOverlap);
}

// Called when the game starts or when spawned
void ABaseLevelTemplate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseLevelTemplate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseLevelTemplate::OnBoxColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBoxColliderOverlap called"));
	if (OtherActor)
	{
		// Проверяем, является ли объект персонажем (или другим классом)
		APawn* OtherPawn = Cast<APawn>(OtherActor);
		if (OtherPawn)
		{
			// Удаление платформы через задержку
			GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &ABaseLevelTemplate::DestroyPlatformWithDelay, 2.0f, false);
		}
	}
}

void ABaseLevelTemplate::DestroyPlatformWithDelay()
{
	// Удаление платформы
	Destroy();

	// Спавн новой платформы
	ASpawnManager* SpawnManager = Cast<ASpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));
	if (SpawnManager)
	{
		// Вызов функции для спавна новой платформы
		SpawnManager->SpawnPlatform();
	}
}

