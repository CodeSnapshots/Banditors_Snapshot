// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Sniper3.h"
#include "Core/TRMacros.h"

UGPC_GR_Sniper3::UGPC_GR_Sniper3()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SNIPER_3));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.0f;
	DeltaProjInitialSpeed = 600.0f;
	DeltaProjMaxSpeed = 600.0f;
}