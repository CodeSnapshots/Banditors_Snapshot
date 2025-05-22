// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Shotgun2.h"
#include "Core/TRMacros.h"

UGPC_BR_Shotgun2::UGPC_BR_Shotgun2()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SHOTGUN_2));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.55f;
	DeltaMissileSpawnedPerShot = 4;
	DeltaDmgEnemyDirect = 9.f;
	DeltaRecoilOffsetRange = 4.2f;
}