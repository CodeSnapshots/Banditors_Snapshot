// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_ST_Rifle1.h"
#include "Core/TRMacros.h"

UGPC_ST_Rifle1::UGPC_ST_Rifle1()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_STOCK_RIFLE_1));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	bOverrideHasDmgDistFallOff = true;
	bHasDmgDistFallOffValue = true;
	DeltaDmgDistFallOffMult = -0.13f;
}