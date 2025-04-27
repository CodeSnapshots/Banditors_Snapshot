// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_SI_Rifle1.h"
#include "Core/TRMacros.h"

UGPC_SI_Rifle1::UGPC_SI_Rifle1()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_SIGHT_RIFLE_1));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaDmgMultOnHead = 0.5f;
}

