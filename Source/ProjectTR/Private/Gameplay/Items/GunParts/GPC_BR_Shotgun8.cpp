// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Shotgun8.h"
#include "Core/TRMacros.h"

UGPC_BR_Shotgun8::UGPC_BR_Shotgun8()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SHOTGUN_8));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.80f;
	DeltaMissileSpawnedPerShot = 6;
	DeltaDmgEnemyDirect = 10.f;
	DeltaRecoilOffsetRange = 4.5f;
}