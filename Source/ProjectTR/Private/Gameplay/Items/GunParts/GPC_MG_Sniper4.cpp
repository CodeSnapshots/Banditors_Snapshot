// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_MG_Sniper4.h"
#include "Core/TRMacros.h"

UGPC_MG_Sniper4::UGPC_MG_Sniper4()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_MAGAZINE_SNIPER_4));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_SINGLE;
}


