// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_SMG6.h"
#include "Core/TRMacros.h"

UGPC_BR_SMG6::UGPC_BR_SMG6()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SMG_6));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.11f;
	DeltaMissileSpawnedPerShot = 1;
	DeltaDmgEnemyDirect = 7.f;
	DeltaRecoilOffsetRange = 1.9f;
}