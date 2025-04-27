// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Items/GunParts/GunPartComponent.h"
#include "GPC_SI_Sniper7.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UGPC_SI_Sniper7 : public UGunPartComponent
{
	GENERATED_BODY()

	UGPC_SI_Sniper7();

public:
	static const EGunPartType GetPartType() { return EGunPartType::EGT_Sight; }
};
