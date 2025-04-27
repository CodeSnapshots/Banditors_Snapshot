// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_MG_Curved2.h"
#include "Core/TRMacros.h"

UGPC_MG_Curved2::UGPC_MG_Curved2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_MAGAZINE_CURVED_2));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP TODO FIXME
	bOverrideFireMode = true;
	FireModeValue = EWeaponFireMode::WFM_AUTO;
}
