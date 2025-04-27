// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_ST_Rifle6.h"
#include "Core/TRMacros.h"

UGPC_ST_Rifle6::UGPC_ST_Rifle6()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_STOCK_RIFLE_6));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	bOverrideHasDmgDistFallOff = true;
	bHasDmgDistFallOffValue = false;
}