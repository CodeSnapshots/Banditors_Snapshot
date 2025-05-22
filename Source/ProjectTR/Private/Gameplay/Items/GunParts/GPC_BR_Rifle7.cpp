// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Rifle7.h"
#include "Core/TRMacros.h"

UGPC_BR_Rifle7::UGPC_BR_Rifle7()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_RIFLE_7));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.05f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 10.f;
	DeltaRecoilOffsetRange = 1.0f;
}