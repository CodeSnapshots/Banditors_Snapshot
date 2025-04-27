// Copyright (C) 2025 by Haguk Kim


#include "UI/BossStateWidget.h"
#include "Characters/GameCharacter.h"

void UBossStateWidget::Bind(AGameCharacter* Boss)
{
	if (!IsValid(Boss))
	{
		UE_LOG(LogTemp, Error, TEXT("UBossStateWidget::Bind - Tried to bind invalid character! Aborting!"));
		return;
	}
	if (BoundBoss.IsValid())
	{
		BoundBoss->UnbindBossState();
	}
	BoundBoss = MakeWeakObjectPtr<AGameCharacter>(Boss);
	BoundBoss->BindBossState(this);
}

void UBossStateWidget::Update()
{
	if (!BoundBoss.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UBossStateWidget::Update - Character is not bound! Removing self from parent! IsNull: %d"), BoundBoss == nullptr);
		RemoveFromParent();
		return;
	}
	if (!WB_TRProgressBar)
	{
		UE_LOG(LogTemp, Error, TEXT("UBossStateWidget::Update - TRProgressBar is invalid! Aborting!"));
		return;
	}
	WB_TRProgressBar->SetPercentage(BoundBoss->GetStat_CurrHealthPercentage());
}
