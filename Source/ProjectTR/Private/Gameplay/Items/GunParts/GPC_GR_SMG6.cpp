// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_SMG6.h"
#include "Core/TRMacros.h"

UGPC_GR_SMG6::UGPC_GR_SMG6()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SMG_6));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.7f;
	DeltaProjInitialSpeed = 1000.0f;
	DeltaProjMaxSpeed = 1000.0f;
}