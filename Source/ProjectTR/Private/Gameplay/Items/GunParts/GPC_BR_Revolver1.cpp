// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Revolver1.h"
#include "Core/TRMacros.h"

UGPC_BR_Revolver1::UGPC_BR_Revolver1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_REVOLVER_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP TODO FIXME
	DeltaDmgEnemyDirect = 25.0f;
	DeltaDmgAllyDirect = 0.0f;
}