// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Sniper5.h"
#include "Core/TRMacros.h"

UGPC_BR_Sniper5::UGPC_BR_Sniper5()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SNIPER_5));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.39f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 19.f;
	DeltaRecoilOffsetRange = 1.1f;
}