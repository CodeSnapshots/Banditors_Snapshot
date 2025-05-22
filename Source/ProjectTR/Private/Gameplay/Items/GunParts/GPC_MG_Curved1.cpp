// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_MG_Curved1.h"
#include "Core/TRMacros.h"

UGPC_MG_Curved1::UGPC_MG_Curved1()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_MAGAZINE_CURVED_1));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP: Tier 1
	DeltaMaxAmmo = 1000;
	DeltaRecoilOffsetRange = 0.0f;
	DeltaAmmoPerShot = 1;
	DeltaFireInterval = 0.0f;
	
	bOverrideExplodeOnHit = false;
	// ...
}
