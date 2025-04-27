// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_SI_Rifle5.h"
#include "Core/TRMacros.h"

UGPC_SI_Rifle5::UGPC_SI_Rifle5()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_SIGHT_RIFLE_5));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);
}

