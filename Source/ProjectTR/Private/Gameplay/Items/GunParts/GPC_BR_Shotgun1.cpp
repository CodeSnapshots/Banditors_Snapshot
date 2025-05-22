// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Shotgun1.h"
#include "Core/TRMacros.h"

UGPC_BR_Shotgun1::UGPC_BR_Shotgun1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SHOTGUN_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.35f;
	DeltaMissileSpawnedPerShot = 2;
	DeltaDmgEnemyDirect = 10.f;
	DeltaRecoilOffsetRange = 3.2f;
}