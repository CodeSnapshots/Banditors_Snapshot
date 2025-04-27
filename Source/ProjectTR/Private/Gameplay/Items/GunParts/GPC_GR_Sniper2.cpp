// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Sniper2.h"
#include "Core/TRMacros.h"

UGPC_GR_Sniper2::UGPC_GR_Sniper2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SNIPER_1));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.0f;
	DeltaProjInitialSpeed = 4000.0f;
	DeltaProjMaxSpeed = 4000.0f;
}