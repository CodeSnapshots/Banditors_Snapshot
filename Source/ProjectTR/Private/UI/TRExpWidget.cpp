// Copyright (C) 2025 by Haguk Kim


#include "UI/TRExpWidget.h"
#include "Characters/GameCharacter.h"
#include "Characters/FPSCharacter.h"
#include "Characters/Components/ExpComponent.h"

void UTRExpWidget::Update(AGameCharacter* Target)
{
	AFPSCharacter* FPSTarget = Cast<AFPSCharacter>(Target);
	if (!IsValid(FPSTarget) || !FPSTarget->ExpComp)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);

		int32 CurrLevel = FPSTarget->ExpComp->GetLevel();
		CurrLevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CurrLevel)));
		float ExpPercent = 0.0f;
		int32 Curr = FPSTarget->ExpComp->GetCurrLevelExp();
		int32 Total = FPSTarget->ExpComp->GetTotalExpReqToLvlup(CurrLevel);
		if (Total > 0)
		{
			ExpPercent = (float) Curr / Total;
		}

		CurrLvlExpText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Curr)));
		LvlUpReqExpText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Total)));

		ExpLeftUntilLvlUpBar->SetPercentage(ExpPercent);
	}
}
