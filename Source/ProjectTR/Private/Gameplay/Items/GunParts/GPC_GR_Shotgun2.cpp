// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Shotgun2.h"
#include "Core/TRMacros.h"

UGPC_GR_Shotgun2::UGPC_GR_Shotgun2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SHOTGUN_2));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 2.0f;
	DeltaProjInitialSpeed = 2200.0f;
	DeltaProjMaxSpeed = 2200.0f;
}