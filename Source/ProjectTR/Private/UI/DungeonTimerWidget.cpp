// Copyright (C) 2024-2025 by Haguk Kim


#include "UI/DungeonTimerWidget.h"
#include "Core/TRGameState.h"

void UDungeonTimerWidget::Update()
{
	if (!GetWorld()) return;
	if (!TimerText) return;
	ATRGameState* TRGS = GetWorld()->GetGameState<ATRGameState>();
	if (!TRGS)
	{
		return;
	}
	TimerText->SetText(FText::FromString(TRGS->GetDungeonTimeString()));
}
