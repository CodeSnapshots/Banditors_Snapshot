// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Sniper6.h"
#include "Core/TRMacros.h"

UGPC_BR_Sniper6::UGPC_BR_Sniper6()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SNIPER_6));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.5f;
	DeltaMissileSpawnedPerShot = 1;
	DeltaDmgEnemyDirect = 15.f;
	DeltaRecoilOffsetRange = 1.0f;
}