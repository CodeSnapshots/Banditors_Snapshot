// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Shotgun5.h"
#include "Core/TRMacros.h"

UGPC_BR_Shotgun5::UGPC_BR_Shotgun5()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SHOTGUN_5));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.48f;
	DeltaMissileSpawnedPerShot = 4;
	DeltaDmgEnemyDirect = 7.f;
	DeltaRecoilOffsetRange = 2.2f;
}