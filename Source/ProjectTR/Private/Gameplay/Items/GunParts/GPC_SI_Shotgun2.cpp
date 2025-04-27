// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_SI_Shotgun2.h"
#include "Core/TRMacros.h"

UGPC_SI_Shotgun2::UGPC_SI_Shotgun2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_SIGHT_SHOTGUN_2));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);
}

