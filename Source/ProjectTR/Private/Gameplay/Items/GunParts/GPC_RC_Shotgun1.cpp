// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_Shotgun1.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypeElemental.h"

UGPC_RC_Shotgun1::UGPC_RC_Shotgun1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_SHOTGUN_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypeElemental::StaticClass();

	DeltaMissileSpawnedPerShot = 2;
	DeltaDmgEnemyDirect = 8.0f;
}