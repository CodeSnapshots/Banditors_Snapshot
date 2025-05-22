// Copyright (C) 2025 by Haguk Kim


#include "UI/GameOverWidget.h"
#include "Core/TRGameState.h"

UGameOverWidget::UGameOverWidget()
{
}

void UGameOverWidget::SetTarget(ATRGameState* Target)
{
	if (!IsValid(Target))
	{
		UE_LOG(LogTemp, Error, TEXT("UTRGameStateHUD::SetTarget - Invalid GameState target! Aborting!"));
		return;
	}
	WidgetTarget = Target;
	UpdateAll();
}

void UGameOverWidget::UpdateAll()
{
	// TODO
}
