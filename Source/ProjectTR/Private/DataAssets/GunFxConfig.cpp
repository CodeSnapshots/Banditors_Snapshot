// Copyright (C) 2024-2025 by Haguk Kim


#include "DataAssets/GunFxConfig.h"

UNiagaraSystem* UGunFxConfig::SearchNiagaraFromEnum(EGunNiagaraReference NiagaraRef) const
{
	switch (NiagaraRef)
	{
	case EGunNiagaraReference::ENR_NULL:
		return nullptr;
	// MZF
	case EGunNiagaraReference::ENR_MZF_Physical_1:
		return MZF_Physical_1;
	case EGunNiagaraReference::ENR_MZF_Physical_2:
		return MZF_Physical_2;
	case EGunNiagaraReference::ENR_MZF_Physical_3:
		return MZF_Physical_3;
	case EGunNiagaraReference::ENR_MZF_Physical_Shotgun_4:
		return MZF_Physical_Shotgun_4;
	case EGunNiagaraReference::ENR_MZF_Plasma_1:
		return MZF_Plasma_1;
	case EGunNiagaraReference::ENR_MZF_Purple_1:
		return MZF_Purple_1;
	case EGunNiagaraReference::ENR_MZF_Energy_1:
		return MZF_Energy_1;
	case EGunNiagaraReference::ENR_MZF_Energy_2:
		return MZF_Energy_2;
	case EGunNiagaraReference::ENR_MZF_Energy_3:
		return MZF_Energy_3;
	case EGunNiagaraReference::ENR_MZF_Laser_1:
		return MZF_Laser_1;
	case EGunNiagaraReference::ENR_MZF_Laser_2:
		return MZF_Laser_2;
	// HIT
	case EGunNiagaraReference::ENR_HIT_BoxFlash_1:
		return HIT_BoxFlash_1;
	case EGunNiagaraReference::ENR_HIT_Physical_1:
		return HIT_Physical_1;
	case EGunNiagaraReference::ENR_HIT_Physical_2:
		return HIT_Physical_2;
	case EGunNiagaraReference::ENR_HIT_Physical_3:
		return HIT_Physical_3;
	case EGunNiagaraReference::ENR_HIT_Physical_Shotgun_4:
		return HIT_Physical_Shotgun_4;
	case EGunNiagaraReference::ENR_HIT_Plasma_1:
		return HIT_Plasma_1;
	case EGunNiagaraReference::ENR_HIT_Purple_1:
		return HIT_Purple_1;
	case EGunNiagaraReference::ENR_HIT_Energy_1:
		return HIT_Energy_1;
	case EGunNiagaraReference::ENR_HIT_Energy_2:
		return HIT_Energy_2;
	case EGunNiagaraReference::ENR_HIT_Energy_3:
		return HIT_Energy_3;
	case EGunNiagaraReference::ENR_HIT_Laser_1:
		return HIT_Laser_1;
	case EGunNiagaraReference::ENR_HIT_Laser_2:
		return HIT_Laser_2;
	// BLT
	case EGunNiagaraReference::ENR_BLT_Default:
		return BLT_Default;
	// SEJ
	case EGunNiagaraReference::ENR_SEJ_Default:
		return SEJ_Default;
	// EXP
	case EGunNiagaraReference::ENR_EXP_Default:
		return EXP_Default;
	default:
		UE_LOG(LogTemp, Error, TEXT("SearchNiagaraFromEnum - Unable to find enum! %d"), NiagaraRef);
		return nullptr;
	}
	return nullptr;
}
