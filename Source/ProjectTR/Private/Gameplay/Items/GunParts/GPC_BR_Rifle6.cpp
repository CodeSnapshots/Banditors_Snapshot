// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Rifle6.h"
#include "Core/TRMacros.h"

UGPC_BR_Rifle6::UGPC_BR_Rifle6()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_RIFLE_6));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.05f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 12.f;
	DeltaRecoilOffsetRange = 2.3f;
}