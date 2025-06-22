// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Pistol1.h"
#include "Core/TRMacros.h"

UGPC_GR_Pistol1::UGPC_GR_Pistol1()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_PISTOL_1));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP: Tier 1
	bOverrideExplodeOnHit = false;
	//bExplodeOnHitValue = ...

	bOverrideDamageType = false;
	//DamageTypeValue = ...

	// Hitscan
	bOverrideGunType = true;
	GunTypeValue = EWeaponFireType::WFT_HITSCAN;

	bOverrideHitscanPiercePawns = true;
	bHitscanPiercePawnsValue = true;

	// VFX
	bSetMuzzleFlashVFXFromEnum = true;
	MuzzleFlashVFXEnum = EGunNiagaraReference::ENR_MZF_Physical_1;
	bSetShellEjectVFXFromEnum = true;
	ShellEjectVFXEnum = EGunNiagaraReference::ENR_SEJ_Default;

	bOverrideApplyLightToMuzzleOnFire = true;
	bApplyLightToMuzzleOnFireValue = true;
	bOverrideMuzzleLightColor = true;
	MuzzleLightColorValue = FColor(255, 186, 102);
	bOverrideMuzzleLightIntensity = true;
	MuzzleLightIntensityValue = 5000.0f;

	// SFX
	// TODO
}