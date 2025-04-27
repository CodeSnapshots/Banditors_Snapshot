// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Items/GunParts/GunPartComponent.h"
#include "Core/TREnums.h"
#include "GPC_BR_DoubleBarrel1.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UGPC_BR_DoubleBarrel1 : public UGunPartComponent
{
	GENERATED_BODY()
	
	UGPC_BR_DoubleBarrel1();

public:
	static const EGunPartType GetPartType() { return EGunPartType::EGT_Barrel; }
};
