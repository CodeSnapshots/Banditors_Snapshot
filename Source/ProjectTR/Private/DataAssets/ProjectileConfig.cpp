// Copyright (C) 2024-2025 by Haguk Kim


#include "DataAssets/ProjectileConfig.h"

TSubclassOf<ABaseProjectile> UProjectileConfig::SearchProjectileFromEnum(EProjectileReference ProjRef) const
{
	switch (ProjRef)
	{
	case EProjectileReference::EPR_NULL:
		return nullptr;
	case EProjectileReference::EPR_DefaultProj:
		return DefaultProjClass;
	default:
		UE_LOG(LogTemp, Error, TEXT("SearchProjectileFromEnum - Unable to find enum! %d"), ProjRef);
		return nullptr;
	}
	return nullptr;
}
