// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_Handgun1.h"
#include "Core/TRMacros.h"

UGPC_RC_Handgun1::UGPC_RC_Handgun1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_HANDGUN_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP TODO FIXME
	bOverrideGunType = true;
	GunTypeValue = EWeaponFireType::WFT_HITSCAN;
	DeltaFireInterval = -0.82f;
}