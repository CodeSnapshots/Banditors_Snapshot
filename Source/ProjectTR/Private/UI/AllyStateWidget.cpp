// Copyright (C) 2025 by Haguk Kim


#include "UI/AllyStateWidget.h"
#include "Core/TRPlayerController.h"
#include "Characters/FPSCharacter.h"

void UAllyStateWidget::Bind(AFPSCharacter* Ally)
{
	if (!IsValid(Ally))
	{
		UE_LOG(LogTemp, Error, TEXT("UAllyStateWidget::Bind - Tried to bind invalid character! Aborting!"));
		return;
	}
	if (BoundPlayer.IsValid())
	{
		BoundPlayer->UnbindAllyState();
	}
	BoundPlayer = MakeWeakObjectPtr<AFPSCharacter>(Ally);
	BoundPlayer->BindAllyState(this);
}

void UAllyStateWidget::Update()
{
	if (!BoundPlayer.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UAllyStateWidget::Update - Character is not bound! Removing self from parent! IsNull: %d"), BoundPlayer == nullptr);
		RemoveFromParent();
		return;
	}
	if (!WB_HealthProgressBar)
	{
		UE_LOG(LogTemp, Error, TEXT("UAllyStateWidget::Update - WB_HealthProgressBar is invalid! Aborting!"));
		return;
	}
	WB_HealthProgressBar->SetPercent(BoundPlayer->GetStat_CurrHealthPercentage());

	if (!PlayerNameText)
	{
		UE_LOG(LogTemp, Error, TEXT("UAllyStateWidget::Update - PlayerNameText is invalid! Aborting!"));
		return;
	}
	FString PlayerName = BoundPlayer->Host_GetName();
	PlayerNameText->SetText(FText::FromString(PlayerName));

	if (!PlayerLevelText)
	{
		UE_LOG(LogTemp, Error, TEXT("UAllyStateWidget::Update - PlayerLevelText is invalid! Aborting!"));
		return;
	}
	FString PlayerLevel = FString::Printf(TEXT("%d"), BoundPlayer->ExpComp->GetLevel());
	PlayerLevelText->SetText(FText::FromString(PlayerLevel));
}
