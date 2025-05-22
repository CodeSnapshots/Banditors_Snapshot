// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_BR_Revolver1.h"
#include "Core/TRMacros.h"

UGPC_BR_Revolver1::UGPC_BR_Revolver1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_BARREL_REVOLVER_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
	DeltaFireInterval = 0.39f;
	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 21.f;
	DeltaRecoilOffsetRange = 2.8f;
}