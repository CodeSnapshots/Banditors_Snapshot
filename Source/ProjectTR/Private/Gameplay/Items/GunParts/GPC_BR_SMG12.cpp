// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_SMG12.h"
#include "Core/TRMacros.h"

UGPC_BR_SMG12::UGPC_BR_SMG12()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SMG_12));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.08f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 5.f;
	DeltaRecoilOffsetRange = 1.2f;
}