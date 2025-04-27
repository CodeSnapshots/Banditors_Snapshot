// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "UI/EscapableWidget.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/EquipSystem.h"
#include "InvBasedWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UInvBasedWidget : public UEscapableWidget
{
	GENERATED_BODY()
	
public:
	// BP에서 정의된 구성요소와 바인딩되어있다
	UPROPERTY(BlueprintReadWrite)
	UInventoryComponent* InvComp = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UEquipSystem* EquSys = nullptr;
};
