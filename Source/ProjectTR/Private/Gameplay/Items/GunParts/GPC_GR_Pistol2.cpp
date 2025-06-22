// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/GunParts/GPC_GR_Pistol2.h"
#include "Core/TRMacros.h"

UGPC_GR_Pistol2::UGPC_GR_Pistol2()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT(SM_GRIP_PISTOL_2));
	UStaticMesh* Asset = MeshAsset.Object;
	SetupMeshComp(Asset, nullptr);

	// TEMP: Tier 1
	bOverrideExplodeOnHit = false;
	//bExplodeOnHitValue = ...

	bOverrideDamageType = false;
	//DamageTypeValue = ...

	// Projectile
	bOverrideGunType = true;
	GunTypeValue = EWeaponFireType::WFT_PROJECTILE;

	bOverrideProjPiercePawns = true;
	bProjPiercePawnsValue = true;

	bSetProjectileFromEnum = true;
	ProjectileEnum = EProjectileReference::EPR_DefaultProj; // TEMP

	bOverrideProjDestroyOnHitCount = true;
	ProjDestroyOnHitCountValue = 1;

	DeltaProjInitialSpeed = 3000.f;
	DeltaProjMaxSpeed = 3000.f;

	bOverrideProjBounceOffObjIndefinitely = false;
	bProjBounceOffObjIndefinitelyValue = false;

	bOverrideProjShouldBounce = false;
	bProjShouldBounceValue = false;

	bOverrideProjBounciness = false;
	ProjBouncinessValue = 0.f;

	bOverrideProjGravityScale = false;
	ProjGravityScaleValue = 1.0f;

	// VFX
	bSetMuzzleFlashVFXFromEnum = true;
	MuzzleFlashVFXEnum = EGunNiagaraReference::ENR_MZF_Energy_1;
	bSetShellEjectVFXFromEnum = true;
	ShellEjectVFXEnum = EGunNiagaraReference::ENR_SEJ_Default;

	bOverrideApplyLightToMuzzleOnFire = true;
	bApplyLightToMuzzleOnFireValue = true;
	bOverrideMuzzleLightColor = true;
	MuzzleLightColorValue = FColor(255, 0, 255);
	bOverrideMuzzleLightIntensity = true;
	MuzzleLightIntensityValue = 5000.0f;

	// SFX
	// TODO
}