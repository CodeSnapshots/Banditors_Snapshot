// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_Shotgun2.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypeMagical.h"

UGPC_RC_Shotgun2::UGPC_RC_Shotgun2()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_SHOTGUN_2));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypeMagical::StaticClass();

	DeltaMissileSpawnedPerShot = 1;
	DeltaDmgEnemyDirect = 30.0f;
}