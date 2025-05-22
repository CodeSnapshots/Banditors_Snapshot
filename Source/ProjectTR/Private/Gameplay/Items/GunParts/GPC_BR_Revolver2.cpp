// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Revolver2.h"
#include "Core/TRMacros.h"

UGPC_BR_Revolver2::UGPC_BR_Revolver2()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_REVOLVER_2));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.33f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 24.f;
	DeltaRecoilOffsetRange = 2.2f;
}