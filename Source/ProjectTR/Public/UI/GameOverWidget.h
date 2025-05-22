// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "UI/TRWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UGameOverWidget : public UTRWidget
{
	GENERATED_BODY()
	
public:
	UGameOverWidget();
	void SetTarget(class ATRGameState* Target);

	void UpdateAll();

public:
	// UI가 나타내는 타깃 (바인딩된 타깃)
	class ATRGameState* WidgetTarget = nullptr;
};
