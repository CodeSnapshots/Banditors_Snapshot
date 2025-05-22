// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_DoubleBarrel1.h"
#include "Core/TRMacros.h"

UGPC_BR_DoubleBarrel1::UGPC_BR_DoubleBarrel1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_DOUBLEBARREL_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.45f;
	DeltaMissileSpawnedPerShot = 1;
	DeltaDmgEnemyDirect = 20.f;
	DeltaRecoilOffsetRange = 2.1f;
}