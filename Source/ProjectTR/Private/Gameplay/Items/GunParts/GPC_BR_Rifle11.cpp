// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Rifle11.h"
#include "Core/TRMacros.h"

UGPC_BR_Rifle11::UGPC_BR_Rifle11()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_RIFLE_11));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.13f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 18.f;
	DeltaRecoilOffsetRange = 1.2f;
}