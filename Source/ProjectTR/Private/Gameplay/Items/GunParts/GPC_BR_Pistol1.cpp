// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Pistol1.h"
#include "Core/TRMacros.h"

UGPC_BR_Pistol1::UGPC_BR_Pistol1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_PISTOL_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.23f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 16.f;
	DeltaRecoilOffsetRange = 1.6f;
}