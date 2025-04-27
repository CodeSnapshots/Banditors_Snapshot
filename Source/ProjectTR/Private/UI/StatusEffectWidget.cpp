// Copyright (C) 2025 by Haguk Kim


#include "UI/StatusEffectWidget.h"
#include "StatusEffect/StatusEffect.h"

void UStatusEffectWidget::BindWithStatusEffect(UStatusEffect* Effect)
{
	FString SEName = Effect->GetStatName();
	if (EffectNameText)
	{
		EffectNameText->SetText(FText::FromString(SEName));
	}

	// TODO: Image
}
