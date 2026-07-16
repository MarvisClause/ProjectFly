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
    DashChargeBar->SetRenderOpacity(Percent > 0.0f ? 1.0f : 0.0f);
    DashChargeBar->SetPercent(Percent);
}