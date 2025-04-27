// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_SMG5.h"
#include "Core/TRMacros.h"

UGPC_GR_SMG5::UGPC_GR_SMG5()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SMG_5));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.4f;
	DeltaProjInitialSpeed = 2600.0f;
	DeltaProjMaxSpeed = 2600.0f;
}