// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_TestBarrel.h"
#include "Core/TRMacros.h"

UGPC_TestBarrel::UGPC_TestBarrel()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_RIFLE_5));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);
}