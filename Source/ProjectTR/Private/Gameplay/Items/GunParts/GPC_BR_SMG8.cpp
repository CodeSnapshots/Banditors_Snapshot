// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_SMG8.h"
#include "Core/TRMacros.h"

UGPC_BR_SMG8::UGPC_BR_SMG8()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SMG_8));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.09f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 8.f;
	DeltaRecoilOffsetRange = 1.4f;
}