// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_ST_Sniper2.h"
#include "Core/TRMacros.h"

UGPC_ST_Sniper2::UGPC_ST_Sniper2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_STOCK_SNIPER_2));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	bOverrideHasDmgDistFallOff = true;
	bHasDmgDistFallOffValue = true;
	DeltaDmgDistFallOffMult = 0.33f;
}