// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_Sniper1.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypeMagical.h"

UGPC_RC_Sniper1::UGPC_RC_Sniper1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_SNIPER_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypeMagical::StaticClass();

	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 27.0f;
}