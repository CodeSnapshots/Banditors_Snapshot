// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_DoubleBarrel3.h"
#include "Core/TRMacros.h"

UGPC_BR_DoubleBarrel3::UGPC_BR_DoubleBarrel3()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_DOUBLEBARREL_3));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.78f;
	DeltaMissileSpawnedPerShot = 6;
	DeltaDmgEnemyDirect = 8.f;
	DeltaRecoilOffsetRange = 5.0f;
}