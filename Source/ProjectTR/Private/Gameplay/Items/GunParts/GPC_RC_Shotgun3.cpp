// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_Shotgun3.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypePhysical.h"

UGPC_RC_Shotgun3::UGPC_RC_Shotgun3()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_SHOTGUN_3));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypePhysical::StaticClass();

	DeltaMissileSpawnedPerShot = 2;
	DeltaDmgEnemyDirect = 15.0f;
}