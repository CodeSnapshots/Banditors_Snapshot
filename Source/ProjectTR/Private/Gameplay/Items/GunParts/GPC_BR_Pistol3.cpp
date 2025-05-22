// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Pistol3.h"
#include "Core/TRMacros.h"

UGPC_BR_Pistol3::UGPC_BR_Pistol3()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_PISTOL_3));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.17f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 11.f;
	DeltaRecoilOffsetRange = 1.3f;
}