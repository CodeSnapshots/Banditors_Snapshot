// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "UI/EscapableWidget.h"
#include "DungeonActors/TRShop.h"
#include "ShopBasedWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UShopBasedWidget : public UEscapableWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	ATRShop* ShopActor = nullptr;
};
