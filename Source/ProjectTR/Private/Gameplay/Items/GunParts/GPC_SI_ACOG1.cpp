// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_SI_ACOG1.h"
#include "Core/TRMacros.h"

UGPC_SI_ACOG1::UGPC_SI_ACOG1()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_SIGHT_ACOG_1));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaDmgMultOnHead = 0.2f;
}


