// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_SMG4.h"
#include "Core/TRMacros.h"

UGPC_BR_SMG4::UGPC_BR_SMG4()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SMG_4));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.1f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 13.f;
	DeltaRecoilOffsetRange = 2.5f;
}