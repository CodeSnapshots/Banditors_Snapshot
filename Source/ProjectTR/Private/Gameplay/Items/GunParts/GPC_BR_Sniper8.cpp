// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Sniper8.h"
#include "Core/TRMacros.h"

UGPC_BR_Sniper8::UGPC_BR_Sniper8()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_SNIPER_8));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.55f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 26.f;
	DeltaRecoilOffsetRange = 0.5f;
}