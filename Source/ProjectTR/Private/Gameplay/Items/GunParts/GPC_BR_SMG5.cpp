// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_SMG5.h"
#include "Core/TRMacros.h"

UGPC_BR_SMG5::UGPC_BR_SMG5()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SMG_5));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.17f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 15.f;
	DeltaRecoilOffsetRange = 2.9f;
}