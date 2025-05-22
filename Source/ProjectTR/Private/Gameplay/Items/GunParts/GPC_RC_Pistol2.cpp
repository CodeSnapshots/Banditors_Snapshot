// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_Pistol2.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypeElemental.h"

UGPC_RC_Pistol2::UGPC_RC_Pistol2()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_PISTOL_2));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypeElemental::StaticClass();

	DeltaMissileSpawnedPerShot = 0;
	DeltaDmgEnemyDirect = 0.0f;
}