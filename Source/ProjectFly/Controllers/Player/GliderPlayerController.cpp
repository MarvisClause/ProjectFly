#include "GliderPlayerController.h"
#include "ProjectFly/Pawns/GliderPawn.h"
#include "ProjectFly/Components/HealthComponent.h"
#include "ProjectFly/Components/FlightPhysicsComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "ProjectFly/Input/GliderInputDataConfig.h"
#include "ProjectFly/Input/GlobalInputDataConfig.h"

#include "ProjectFly/UI/Glider/GliderHUDWidget.h"

#include "ProjectFly/Objects/HintMessageTrigger.h"

#include <Kismet/GameplayStatics.h>

void AGliderPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Get glider pawn
    GliderPawn = Cast<AGliderPawn>(GetPawn());

    // Get local player and add mapping context
    auto* LocalPlayer = GetLocalPlayer();
    UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(GlobalContext, 0);
    Subsystem->AddMappingContext(GliderContext, 1);

	// Initialize widget
	HUDWidget = CreateWidget<UGliderHUDWidget>(this, HUDClass);
	HUDWidget->AddToViewport();
	HUDWidget->SetVisibility(ESlateVisibility::Visible);

	// Find all hint message triggers and subsribe to them
	TArray<AActor*> HintTriggers;
	UGameplayStatics::GetAllActorsOfClass(this, AHintMessageTrigger::StaticClass(), HintTriggers);
	for (AActor* Actor : HintTriggers)
	{
		if (AHintMessageTrigger* HintTrigger = Cast<AHintMessageTrigger>(Actor))
		{
			HintTrigger->OnHintTriggered.AddDynamic( this, &AGliderPlayerController::HandleHintTriggered );
		}
	}
}

void AGliderPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GliderPawn.Get())
	{
		HUDWidget->SetHealth(GliderPawn->AccessHealthComponent()->CurrentHealth);
		HUDWidget->SetSpeed(GliderPawn->AccessFlightPhysicsComponent()->GetSpeedPercentage());
		HUDWidget->SetStamina(GliderPawn->GetDashStamina());
		HUDWidget->SetDashCharge(GliderPawn->GetDashCharge());
	}
}

void AGliderPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    auto* Input = CastChecked<UEnhancedInputComponent>(InputComponent);

	if (!Input)
	{
		return;
	}

	// Bind glider logic
	Input->BindAction(GliderActions->LookAction, ETriggerEvent::Triggered, this, &AGliderPlayerController::OnLook);

	Input->BindAction(GliderActions->PitchAction, ETriggerEvent::Triggered, this, &AGliderPlayerController::OnPitch);
	Input->BindAction(GliderActions->YawAction, ETriggerEvent::Triggered, this, &AGliderPlayerController::OnYaw);
	Input->BindAction(GliderActions->RollAction, ETriggerEvent::Triggered, this, &AGliderPlayerController::OnRoll);

	Input->BindAction(GliderActions->DashAction, ETriggerEvent::Started, this, &AGliderPlayerController::OnDashStarted);
	Input->BindAction(GliderActions->DashAction, ETriggerEvent::Completed, this, &AGliderPlayerController::OnDashReleased);

	Input->BindAction(GliderActions->HaltAction, ETriggerEvent::Started, this, &AGliderPlayerController::OnHaltStarted);
	Input->BindAction(GliderActions->HaltAction, ETriggerEvent::Completed, this, &AGliderPlayerController::OnHaltReleased);

	Input->BindAction(GliderActions->FreeLookAction, ETriggerEvent::Started, this, &AGliderPlayerController::OnFreeLookStarted);
	Input->BindAction(GliderActions->FreeLookAction, ETriggerEvent::Completed, this, &AGliderPlayerController::OnFreeLookReleased);

	// Bind global context
	Input->BindAction(GlobalActions->PauseAction, ETriggerEvent::Started, this,	&AGliderPlayerController::TogglePause);
}

void AGliderPlayerController::EnableGameplayInput()
{
	auto* LocalPlayer = GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	Subsystem->AddMappingContext(GliderContext, 1);
}

void AGliderPlayerController::DisableGameplayInput()
{
	auto* LocalPlayer = GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	Subsystem->RemoveMappingContext(GliderContext);
}

void AGliderPlayerController::TogglePause()
{
	// ...
}

void AGliderPlayerController::OnLook(const FInputActionValue& Value)
{
	if (GliderPawn.Get())
	{
		GliderPawn->LookChange(Value.Get<FVector2D>());
	}
}

void AGliderPlayerController::OnPitch(const FInputActionValue& Value)
{
	if ( GliderPawn.Get() )
	{
		GliderPawn->MovePitch(Value.Get<float>());
	}
}

void AGliderPlayerController::OnYaw(const FInputActionValue& Value)
{
	if (GliderPawn.Get())
	{
		GliderPawn->MoveYaw(Value.Get<float>());
	}
}

void AGliderPlayerController::OnRoll(const FInputActionValue& Value)
{
	if (GliderPawn.Get())
	{
		GliderPawn->MoveRoll(Value.Get<float>());
	}
}

void AGliderPlayerController::OnDashStarted(const FInputActionValue&)
{
	if (GliderPawn.Get())
	{
		GliderPawn->StartDash();
	}
}

void AGliderPlayerController::OnDashReleased(const FInputActionValue&)
{
	if (GliderPawn.Get())
	{
		GliderPawn->ReleaseDash();
	}
}

void AGliderPlayerController::OnHaltStarted(const FInputActionValue&)
{
	if (GliderPawn.Get())
	{
		GliderPawn->StartHalt();
	}
}

void AGliderPlayerController::OnHaltReleased(const FInputActionValue&)
{
	if (GliderPawn.Get())
	{
		GliderPawn->StopHalt();
	}
}

void AGliderPlayerController::OnFreeLookStarted(const FInputActionValue&)
{
	if (GliderPawn.Get())
	{
		GliderPawn->StartFreeLook();
	}
}

void AGliderPlayerController::OnFreeLookReleased(const FInputActionValue&)
{
	if (GliderPawn.Get())
	{
		GliderPawn->StopFreeLook();
	}
}

void AGliderPlayerController::HandleHintTriggered(FText Message, float DisplayTime)
{
	HUDWidget->SetHintMessage(Message, DisplayTime);
}
