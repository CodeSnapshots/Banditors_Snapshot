// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_MZ_SMG2.h"
#include "Core/TRMacros.h"

UGPC_MZ_SMG2::UGPC_MZ_SMG2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_MUZZLE_SMG_2));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);
}

