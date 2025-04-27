// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_Rifle2.h"
#include "Core/TRMacros.h"

UGPC_RC_Rifle2::UGPC_RC_Rifle2()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_RIFLE_2));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP TODO FIXME
	bOverrideGunType = true;
	GunTypeValue = EWeaponFireType::WFT_HITSCAN;
	DeltaFireInterval = -0.88f;
}