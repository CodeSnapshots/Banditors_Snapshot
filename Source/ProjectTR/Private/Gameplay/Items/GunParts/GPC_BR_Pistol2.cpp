// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Pistol2.h"
#include "Core/TRMacros.h"

UGPC_BR_Pistol2::UGPC_BR_Pistol2()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_PISTOL_2));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.25f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 20.f;
	DeltaRecoilOffsetRange = 0.9f;
}