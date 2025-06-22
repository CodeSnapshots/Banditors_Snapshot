// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TierFxConfig.generated.h"

UENUM(BlueprintType)
enum class ETierNiagaraReference : uint8
{
	ENR_NULL,

	ENR_TIER_1_LOOP,
	ENR_TIER_2_LOOP,
	ENR_TIER_3_LOOP,
	ENR_TIER_4_LOOP,
	ENR_TIER_5_LOOP,

	ENR_TIER_1_SPAWN,
	ENR_TIER_2_SPAWN,
	ENR_TIER_3_SPAWN,
	ENR_TIER_4_SPAWN,
	ENR_TIER_5_SPAWN,

	ENR_OBJECTIVE_LOOP,
	ENR_OBJECTIVE_SPAWN,
};

/**
 * 
 */
UCLASS()
class PROJECTTR_API UTierFxConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UNiagaraSystem* SearchNiagaraFromEnum(ETierNiagaraReference NiagaraRef) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T1_Loop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T2_Loop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T3_Loop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T4_Loop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T5_Loop;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T1_Spawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T2_Spawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T3_Spawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T4_Spawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_T5_Spawn;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_Objective_Loop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TFX_Objective_Spawn;
};
