// Copyright (C) 2025 by Haguk Kim


#include "UI/RoomKeyWidget.h"
#include "Core/TRGameState.h"

void URoomKeyWidget::Update(int32 Count)
{
	SetKeyCount(Count);
}

void URoomKeyWidget::SetKeyCount(int32 Count)
{
	check(KeyCountText);
	KeyCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Count)));
}
