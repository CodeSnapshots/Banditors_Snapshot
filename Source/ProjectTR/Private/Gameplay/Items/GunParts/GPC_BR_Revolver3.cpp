// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Revolver3.h"
#include "Core/TRMacros.h"

UGPC_BR_Revolver3::UGPC_BR_Revolver3()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_REVOLVER_3));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP TODO FIXME
	DeltaDmgEnemyDirect = 20.0f;
	DeltaDmgAllyDirect = 20.0f;
}