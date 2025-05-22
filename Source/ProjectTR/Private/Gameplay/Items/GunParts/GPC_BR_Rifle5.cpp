// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Rifle5.h"
#include "Core/TRMacros.h"

UGPC_BR_Rifle5::UGPC_BR_Rifle5()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_RIFLE_5));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.07f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 15.f;
	DeltaRecoilOffsetRange = 2.5f;
}