// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_SMG2.h"
#include "Core/TRMacros.h"

UGPC_GR_SMG2::UGPC_GR_SMG2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SMG_2));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.8f;
	DeltaProjInitialSpeed = 1500.0f;
	DeltaProjMaxSpeed = 1500.0f;
}