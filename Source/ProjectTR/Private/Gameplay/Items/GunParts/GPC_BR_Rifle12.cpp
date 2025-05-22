// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Rifle12.h"
#include "Core/TRMacros.h"

UGPC_BR_Rifle12::UGPC_BR_Rifle12()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_RIFLE_12));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.25f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 22.f;
	DeltaRecoilOffsetRange = 2.1f;
}