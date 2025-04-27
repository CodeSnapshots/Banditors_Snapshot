// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Items/GunParts/GunPartComponent.h"
#include "GPC_MG_SMG1.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UGPC_MG_SMG1 : public UGunPartComponent
{
	GENERATED_BODY()

	UGPC_MG_SMG1();

public:
	static const EGunPartType GetPartType() { return EGunPartType::EGT_Magazine; }
};
