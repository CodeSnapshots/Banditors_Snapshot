// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Rifle7.h"
#include "Core/TRMacros.h"

UGPC_GR_Rifle7::UGPC_GR_Rifle7()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_RIFLE_7));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.4f;
	DeltaProjInitialSpeed = 3600.0f;
	DeltaProjMaxSpeed = 3600.0f;
}