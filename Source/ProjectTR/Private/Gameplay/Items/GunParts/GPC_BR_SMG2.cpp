// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_SMG2.h"
#include "Core/TRMacros.h"

UGPC_BR_SMG2::UGPC_BR_SMG2()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SMG_2));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.12f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 8.f;
	DeltaRecoilOffsetRange = 1.0f;
}