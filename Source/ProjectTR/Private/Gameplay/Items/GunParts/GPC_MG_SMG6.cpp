// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_MG_SMG6.h"
#include "Core/TRMacros.h"

UGPC_MG_SMG6::UGPC_MG_SMG6()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_MAGAZINE_SMG_6));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
}


