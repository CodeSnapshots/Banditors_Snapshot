// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Items/GunParts/GunPartComponent.h"
#include "GPC_RC_Shotgun2.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UGPC_RC_Shotgun2 : public UGunPartComponent
{
	GENERATED_BODY()
	
	UGPC_RC_Shotgun2();

public:
	static const EGunPartType GetPartType() { return EGunPartType::EGT_Receiver; }
};
