// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Items/GunParts/GunPartComponent.h"
#include "GPC_BR_Pistol3.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UGPC_BR_Pistol3 : public UGunPartComponent
{
	GENERATED_BODY()
	
	UGPC_BR_Pistol3();

public:
	static const EGunPartType GetPartType() { return EGunPartType::EGT_Barrel; }
};
