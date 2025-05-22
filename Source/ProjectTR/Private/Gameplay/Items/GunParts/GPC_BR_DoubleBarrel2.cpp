// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_DoubleBarrel2.h"
#include "Core/TRMacros.h"

UGPC_BR_DoubleBarrel2::UGPC_BR_DoubleBarrel2()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_DOUBLEBARREL_2));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.67f;
	DeltaMissileSpawnedPerShot = 4;
	DeltaDmgEnemyDirect = 12.f;
	DeltaRecoilOffsetRange = 3.3f;
}