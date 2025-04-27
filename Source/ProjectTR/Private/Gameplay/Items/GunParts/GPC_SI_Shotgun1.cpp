// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_SI_Shotgun1.h"
#include "Core/TRMacros.h"

UGPC_SI_Shotgun1::UGPC_SI_Shotgun1()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_SIGHT_SHOTGUN_1));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaDmgMultOnHead = 0.5f;
}

