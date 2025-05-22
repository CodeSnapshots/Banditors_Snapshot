// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Revolver3.h"
#include "Core/TRMacros.h"

UGPC_BR_Revolver3::UGPC_BR_Revolver3()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_REVOLVER_3));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.35f;
	DeltaMissileSpawnedPerShot = 1;
	DeltaDmgEnemyDirect = 17.f;
	DeltaRecoilOffsetRange = 1.9f;
}