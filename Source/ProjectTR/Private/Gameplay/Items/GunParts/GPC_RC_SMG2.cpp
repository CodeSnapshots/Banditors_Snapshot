// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_SMG2.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypePhysical.h"

UGPC_RC_SMG2::UGPC_RC_SMG2()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_SMG_2));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypePhysical::StaticClass();

	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 4.0f;
}