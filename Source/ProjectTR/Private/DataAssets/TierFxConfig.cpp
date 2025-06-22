// Copyright (C) 2025 by Haguk Kim


#include "DataAssets/TierFxConfig.h"

UNiagaraSystem* UTierFxConfig::SearchNiagaraFromEnum(ETierNiagaraReference NiagaraRef) const
{
	switch (NiagaraRef)
	{
	case ETierNiagaraReference::ENR_NULL:
		return nullptr;

	case ETierNiagaraReference::ENR_TIER_1_LOOP:
		return TFX_T1_Loop;
	case ETierNiagaraReference::ENR_TIER_2_LOOP:
		return TFX_T2_Loop;
	case ETierNiagaraReference::ENR_TIER_3_LOOP:
		return TFX_T3_Loop;
	case ETierNiagaraReference::ENR_TIER_4_LOOP:
		return TFX_T4_Loop;
	case ETierNiagaraReference::ENR_TIER_5_LOOP:
		return TFX_T5_Loop;

	case ETierNiagaraReference::ENR_TIER_1_SPAWN:
		return TFX_T1_Spawn;
	case ETierNiagaraReference::ENR_TIER_2_SPAWN:
		return TFX_T2_Spawn;
	case ETierNiagaraReference::ENR_TIER_3_SPAWN:
		return TFX_T3_Spawn;
	case ETierNiagaraReference::ENR_TIER_4_SPAWN:
		return TFX_T4_Spawn;
	case ETierNiagaraReference::ENR_TIER_5_SPAWN:
		return TFX_T5_Spawn;

	case ETierNiagaraReference::ENR_OBJECTIVE_LOOP:
		return TFX_Objective_Loop;
	case ETierNiagaraReference::ENR_OBJECTIVE_SPAWN:
		return TFX_Objective_Spawn;
	default:
		return nullptr;
	}
	return nullptr;
}
