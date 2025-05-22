// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Rifle3.h"
#include "Core/TRMacros.h"

UGPC_BR_Rifle3::UGPC_BR_Rifle3()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_RIFLE_3));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.09f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 20.f;
	DeltaRecoilOffsetRange = 3.0f;
}