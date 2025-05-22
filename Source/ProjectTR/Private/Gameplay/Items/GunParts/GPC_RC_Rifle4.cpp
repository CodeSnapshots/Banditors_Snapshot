// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_Rifle4.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypeMagical.h"

UGPC_RC_Rifle4::UGPC_RC_Rifle4()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_RIFLE_4));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypeMagical::StaticClass();

	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 0.0f;
}