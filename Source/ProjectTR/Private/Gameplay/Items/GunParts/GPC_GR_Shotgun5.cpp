// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Shotgun5.h"
#include "Core/TRMacros.h"

UGPC_GR_Shotgun5::UGPC_GR_Shotgun5()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SHOTGUN_5));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 1.5f;
	DeltaProjInitialSpeed = 1500.0f;
	DeltaProjMaxSpeed = 1500.0f;
}