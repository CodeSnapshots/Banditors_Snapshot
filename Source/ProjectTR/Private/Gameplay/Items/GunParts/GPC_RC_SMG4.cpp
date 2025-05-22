// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_SMG4.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypeElemental.h"

UGPC_RC_SMG4::UGPC_RC_SMG4()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_SMG_4));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypeElemental::StaticClass();

	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 2.0f;
}