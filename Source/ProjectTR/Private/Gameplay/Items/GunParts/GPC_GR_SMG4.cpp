// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_SMG4.h"
#include "Core/TRMacros.h"

UGPC_GR_SMG4::UGPC_GR_SMG4()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_SMG_4));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.05f;
	DeltaProjInitialSpeed = 4600.0f;
	DeltaProjMaxSpeed = 4600.0f;
}