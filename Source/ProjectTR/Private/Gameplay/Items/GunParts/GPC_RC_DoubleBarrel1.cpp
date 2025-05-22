// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_RC_DoubleBarrel1.h"
#include "Core/TRMacros.h"
#include "Gameplay/Damage/DamageTypePhysical.h"

UGPC_RC_DoubleBarrel1::UGPC_RC_DoubleBarrel1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT(SK_RECEIVER_DOUBLEBARREL_1));
	USkeletalMesh* Asset = MeshAsset.Object;
	SetupMeshComp(nullptr, Asset);

	// TEMP: Tier 1
	bOverrideDamageType = true;
	DamageTypeValue = UDamageTypePhysical::StaticClass();

	DeltaMissileSpawnedPerShot = 1;
	DeltaDmgEnemyDirect = 25.0f;
	DeltaFireInterval = 0.2f;
}