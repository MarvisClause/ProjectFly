#include "GliderHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include <Kismet/GameplayStatics.h>

void UGliderHUDWidget::SetHealth(float Health)
{
    HealthText->SetText(FText::AsNumber(Health));
}

void UGliderHUDWidget::SetSpeed(float Speed)
{
    SpeedText->SetText(FText::AsNumber(Speed));
}

void UGliderHUDWidget::SetStamina(float Stamina)
{
    StaminaText->SetText(FText::AsNumber(Stamina));
}

void UGliderHUDWidget::SetDashCharge(float Percent)
{
    DashChargeText->SetText(FText::AsNumber(Percent));
}