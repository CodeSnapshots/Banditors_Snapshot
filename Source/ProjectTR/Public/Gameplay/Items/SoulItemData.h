// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Core/TRStructs.h"
#include "Items/ItemData.h"
#include "SoulItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API USoulItemData : public UItemData
{
	GENERATED_BODY()
	
	USoulItemData();

public:
	virtual bool CacheItem(const class ABaseItem* Item) override;

	class ATRPlayerController* GetCachedController() const { return Server_Controller; }
	void CacheController(class ATRPlayerController* Controller) { Server_Controller = Controller; }

protected:
	// 부활 대상 플레이어
	UPROPERTY(BlueprintReadOnly)
	class ATRPlayerController* Server_Controller = nullptr;
};
