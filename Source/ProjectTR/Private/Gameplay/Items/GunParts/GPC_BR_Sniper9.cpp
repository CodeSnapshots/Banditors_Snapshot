// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Sniper9.h"
#include "Core/TRMacros.h"

UGPC_BR_Sniper9::UGPC_BR_Sniper9()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SNIPER_9));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.7f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 29.f;
	DeltaRecoilOffsetRange = 0.2f;
}