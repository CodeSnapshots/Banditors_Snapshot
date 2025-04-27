// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_MZ_Any9.h"
#include "Core/TRMacros.h"

UGPC_MZ_Any9::UGPC_MZ_Any9()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_MUZZLE_ANY_9));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);
}

