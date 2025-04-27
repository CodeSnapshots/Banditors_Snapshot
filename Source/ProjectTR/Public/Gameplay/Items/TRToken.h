// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Items/BaseItem.h"
#include "TRToken.generated.h"

UCLASS()
class PROJECTTR_API ATRToken : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ATRToken();
	void Initialize(int32 TokenTier);

protected:
	virtual void BeginPlay() override;

public:
	/* Getter */
	UFUNCTION()
	int32 GetTier() { return Tier; }

protected:
	// 토큰 등급
	UPROPERTY(BlueprintReadOnly)
	int32 Tier = 0;
};
