// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Sniper4.h"
#include "Core/TRMacros.h"

UGPC_GR_Sniper4::UGPC_GR_Sniper4()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SNIPER_4));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 25.0f;
	DeltaProjInitialSpeed = 2000.0f;
	DeltaProjMaxSpeed = 2000.0f;
}