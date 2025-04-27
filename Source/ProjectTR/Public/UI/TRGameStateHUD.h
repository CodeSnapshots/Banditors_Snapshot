// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"

#include "UI/DungeonTimerWidget.h"
#include "UI/RoomKeyWidget.h"
#include "UI/BossfightWidget.h"
#include "UI/TRWidget.h"
#include "TRGameStateHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UTRGameStateHUD : public UTRWidget
{
	GENERATED_BODY()

public:
	UTRGameStateHUD();
	void SetTarget(class ATRGameState* Target);

/* 위젯 업데이트 */
public:
	void UpdateDungeonDepth();
	void UpdateDungeonTimer();
	void UpdateRoomKeys();
	void UpdateBossfight();

	// 일괄 업데이트; 타깃 교체 시 자동으로 호출된다
	void UpdateAll();

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DungeonDepthText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonTimerWidget* WB_DungeonTimer = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	URoomKeyWidget* WB_RoomKeyWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBossfightWidget* WB_BossfightWidget = nullptr;

public:
	// UI가 나타내는 타깃 (바인딩된 타깃)
	class ATRGameState* HudTarget = nullptr;
};
