// Copyright (C) 2024-2025 by Haguk Kim


#include "UI/PlayerNameTagWidget.h"

void UPlayerNameTagWidget::SetName(FString Name)
{
	if (NameTagTextBox)
	{
		NameTagTextBox->SetText(FText::FromString(Name));
	}
}

FString UPlayerNameTagWidget::GetNameText() const
{
	if (NameTagTextBox)
	{
		return NameTagTextBox->GetText().ToString();
	}
	return FString("");
}
