#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HintMessageWidget.generated.h"

class UTextBlock;

UCLASS(Abstract)
class PROJECTFLY_API UHintMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowMessage(const FText& Message, float DisplayTime);

protected:
	virtual void NativeConstruct() override;

private:
	void HideMessage();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HintText;

	FTimerHandle HideHintMessageTimer;
};