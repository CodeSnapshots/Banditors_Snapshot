// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Items/GunParts/GunPartComponent.h"
#include "GPC_MZ_Any5.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UGPC_MZ_Any5 : public UGunPartComponent
{
	GENERATED_BODY()

	UGPC_MZ_Any5();

public:
	static const EGunPartType GetPartType() { return EGunPartType::EGT_Muzzle; }
};
