// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Pistol3.h"
#include "Core/TRMacros.h"

UGPC_BR_Pistol3::UGPC_BR_Pistol3()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_PISTOL_3));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP TODO FIXME
	DeltaDmgEnemyDirect = 18.0f;
	DeltaDmgAllyDirect = 0.0f;
}