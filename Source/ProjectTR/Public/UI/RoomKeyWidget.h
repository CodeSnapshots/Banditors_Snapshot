// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"

#include "UI/TRWidget.h"
#include "RoomKeyWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API URoomKeyWidget : public UTRWidget
{
	GENERATED_BODY()

public:
	void Update(int32 Count);

protected:
	void SetKeyCount(int32 Count);
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* KeyCountText = nullptr;
};
