#include "HintMessageWidget.h"

#include "Components/TextBlock.h"
#include "TimerManager.h"

void UHintMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(HintText);

	SetVisibility(ESlateVisibility::Hidden);
}

void UHintMessageWidget::ShowMessage(const FText& Message, float DisplayTime)
{
	HintText->SetText(Message);

	SetVisibility(ESlateVisibility::Visible);

	// Restart timer, if another message was already being displayed
	GetWorld()->GetTimerManager().ClearTimer(HideHintMessageTimer);
	GetWorld()->GetTimerManager().SetTimer(HideHintMessageTimer, this, &UHintMessageWidget::HideMessage, DisplayTime, false);
}

void UHintMessageWidget::HideMessage()
{
	SetVisibility(ESlateVisibility::Hidden);
}