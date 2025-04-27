// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_SI_Rifle3.h"
#include "Core/TRMacros.h"

UGPC_SI_Rifle3::UGPC_SI_Rifle3()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_SIGHT_RIFLE_3));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);
}

