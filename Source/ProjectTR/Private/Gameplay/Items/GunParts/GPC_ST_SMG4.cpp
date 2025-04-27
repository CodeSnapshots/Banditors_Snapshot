// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_ST_SMG4.h"
#include "Core/TRMacros.h"

UGPC_ST_SMG4::UGPC_ST_SMG4()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_STOCK_SMG_4));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	bOverrideHasDmgDistFallOff = true;
	bHasDmgDistFallOffValue = true;
	DeltaDmgDistFallOffMult = -0.88f;
}