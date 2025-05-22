// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_SMG10.h"
#include "Core/TRMacros.h"

UGPC_BR_SMG10::UGPC_BR_SMG10()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SMG_10));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.1f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 11.f;
	DeltaRecoilOffsetRange = 2.4f;
}