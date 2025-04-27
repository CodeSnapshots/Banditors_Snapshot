// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Rifle4.h"
#include "Core/TRMacros.h"

UGPC_GR_Rifle4::UGPC_GR_Rifle4()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_RIFLE_4));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.9f;
	DeltaProjInitialSpeed = 2600.0f;
	DeltaProjMaxSpeed = 2600.0f;
}