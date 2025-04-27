// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Rifle1.h"
#include "Core/TRMacros.h"

UGPC_GR_Rifle1::UGPC_GR_Rifle1()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_RIFLE_1));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.1f;
	DeltaProjInitialSpeed = 3000.0f;
	DeltaProjMaxSpeed = 3000.0f;
}