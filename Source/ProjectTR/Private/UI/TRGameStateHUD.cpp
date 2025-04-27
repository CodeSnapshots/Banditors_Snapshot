// Copyright (C) 2025 by Haguk Kim


#include "UI/TRGameStateHUD.h"
#include "Core/TRGameState.h"
#include "Characters/GameCharacter.h"

UTRGameStateHUD::UTRGameStateHUD()
{
}

void UTRGameStateHUD::SetTarget(ATRGameState* Target)
{
	if (!IsValid(Target))
	{
		UE_LOG(LogTemp, Error, TEXT("UTRGameStateHUD::SetTarget - Invalid GameState target! Aborting!"));
		return;
	}
	HudTarget = Target;
	UpdateAll();
}

void UTRGameStateHUD::UpdateDungeonDepth()
{
	if (!DungeonDepthText) return;
	UWorld* World = GetWorld();
	if (!World) return;

	ATRPlayerController* PC = Cast<ATRPlayerController>(GetGameInstance() ? GetGameInstance()->GetFirstLocalPlayerController() : nullptr);
	if (!IsValid(PC))
	{
		DungeonDepthText->SetText(FText::FromString(""));
	}
	else
	{
		DungeonDepthText->SetText(FText::FromString(FString::Printf(TEXT("%d"), PC->Local_GetCurrDungeonDepth())));
	}
}

void UTRGameStateHUD::UpdateDungeonTimer()
{
	if (!WB_DungeonTimer) return;
	WB_DungeonTimer->Update();
}

void UTRGameStateHUD::UpdateRoomKeys()
{
	if (!WB_RoomKeyWidget) return;
	if (!HudTarget) return;
	WB_RoomKeyWidget->Update(HudTarget->Host_GetDoorKeyCount());
}

void UTRGameStateHUD::UpdateBossfight()
{
	if (!WB_BossfightWidget) return;
	if (!HudTarget) return;
	WB_BossfightWidget->Update(HudTarget->Local_GetBossCharacters());
}

void UTRGameStateHUD::UpdateAll()
{
	UpdateDungeonDepth();
	UpdateDungeonTimer();
	UpdateRoomKeys();
	UpdateBossfight();
}
