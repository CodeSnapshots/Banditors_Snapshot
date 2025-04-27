// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Rifle2.h"
#include "Core/TRMacros.h"

UGPC_GR_Rifle2::UGPC_GR_Rifle2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_RIFLE_2));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.3f;
	DeltaProjInitialSpeed = 3300.0f;
	DeltaProjMaxSpeed = 3300.0f;
}