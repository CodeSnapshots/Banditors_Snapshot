// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_MZ_Any4.h"
#include "Core/TRMacros.h"

UGPC_MZ_Any4::UGPC_MZ_Any4()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_MUZZLE_ANY_4));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);
}

