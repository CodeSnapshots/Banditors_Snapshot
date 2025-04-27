// Copyright (C) 2025 by Haguk Kim


#include "UI/TRShardWidget.h"
#include "Characters/GameCharacter.h"
#include "Characters/FPSCharacter.h"
#include "Characters/Components/ExpComponent.h"

void UTRShardWidget::Update(AGameCharacter* Target)
{
	AFPSCharacter* FPSTarget = Cast<AFPSCharacter>(Target);
	if (IsValid(FPSTarget) && FPSTarget->ExpComp)
	{
		int32 Shards = FPSTarget->ExpComp->GetCurrShard();
		ShardCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Shards)));
	}
	else
	{
		ShardCountText->SetText(FText::FromString(FString("0")));
	}
}
