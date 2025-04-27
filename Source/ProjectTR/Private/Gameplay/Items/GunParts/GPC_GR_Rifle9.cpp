// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Rifle9.h"
#include "Core/TRMacros.h"

UGPC_GR_Rifle9::UGPC_GR_Rifle9()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_RIFLE_9));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	DeltaRecoilOffsetRange = 0.0f;
	DeltaProjInitialSpeed = 800.0f;
	DeltaProjMaxSpeed = 800.0f;
}