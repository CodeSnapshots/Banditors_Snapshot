// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Items/GunParts/GunPartComponent.h"
#include "GPC_ST_Rifle9.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UGPC_ST_Rifle9 : public UGunPartComponent
{
	GENERATED_BODY()
	
	UGPC_ST_Rifle9();

public:
	static const EGunPartType GetPartType() { return EGunPartType::EGT_Stock; }
};
