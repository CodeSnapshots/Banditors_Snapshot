// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Items/GunParts/GunPartComponent.h"
#include "GPC_RC_Rifle3.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UGPC_RC_Rifle3 : public UGunPartComponent
{
	GENERATED_BODY()
	
	UGPC_RC_Rifle3();

public:
	static const EGunPartType GetPartType() { return EGunPartType::EGT_Receiver; }
};
