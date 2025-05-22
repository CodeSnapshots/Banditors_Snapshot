// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Shotgun4.h"
#include "Core/TRMacros.h"

UGPC_BR_Shotgun4::UGPC_BR_Shotgun4()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SHOTGUN_4));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.28f;
	DeltaMissileSpawnedPerShot = 1;
	DeltaDmgEnemyDirect = 8.f;
	DeltaRecoilOffsetRange = 1.9f;
}