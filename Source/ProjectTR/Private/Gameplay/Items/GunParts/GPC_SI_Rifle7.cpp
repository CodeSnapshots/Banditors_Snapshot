// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_SI_Rifle7.h"
#include "Core/TRMacros.h"

UGPC_SI_Rifle7::UGPC_SI_Rifle7()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_SIGHT_RIFLE_7));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);
}

