// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Sniper1.h"
#include "Core/TRMacros.h"

UGPC_BR_Sniper1::UGPC_BR_Sniper1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SNIPER_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.43f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 20.f;
	DeltaRecoilOffsetRange = 0.5f;
}