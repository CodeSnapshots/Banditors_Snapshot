// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_MG_Pistol6.h"
#include "Core/TRMacros.h"

UGPC_MG_Pistol6::UGPC_MG_Pistol6()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_MAGAZINE_PISTOL_6));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
}

