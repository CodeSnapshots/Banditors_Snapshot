// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Shotgun4.h"
#include "Core/TRMacros.h"

UGPC_GR_Shotgun4::UGPC_GR_Shotgun4()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SHOTGUN_4));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 4.0f;
	DeltaProjInitialSpeed = 2900.0f;
	DeltaProjMaxSpeed = 2900.0f;
}