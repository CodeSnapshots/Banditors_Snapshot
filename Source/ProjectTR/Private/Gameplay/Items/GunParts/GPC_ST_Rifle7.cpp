// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_ST_Rifle7.h"
#include "Core/TRMacros.h"

UGPC_ST_Rifle7::UGPC_ST_Rifle7()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_STOCK_RIFLE_7));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	bOverrideHasDmgDistFallOff = true;
	bHasDmgDistFallOffValue = true;
	DeltaDmgDistFallOffMult = -0.05f;
}