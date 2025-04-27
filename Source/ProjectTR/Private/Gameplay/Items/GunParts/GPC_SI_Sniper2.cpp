// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_SI_Sniper2.h"
#include "Core/TRMacros.h"

UGPC_SI_Sniper2::UGPC_SI_Sniper2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_SIGHT_SNIPER_2));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaDmgMultOnHead = 1.5f;
}

