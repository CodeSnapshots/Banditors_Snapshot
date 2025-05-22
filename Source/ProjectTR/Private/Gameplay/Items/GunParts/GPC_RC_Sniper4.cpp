// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_Sniper4.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypeElemental.h"

UGPC_RC_Sniper4::UGPC_RC_Sniper4()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_SNIPER_4));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypeElemental::StaticClass();

	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 16.0f;
}