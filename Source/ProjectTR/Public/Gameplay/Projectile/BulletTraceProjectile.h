// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"

#include "Projectile/BaseProjectile.h"
#include "BulletTraceProjectile.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ABulletTraceProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
protected:
	ABulletTraceProjectile();

	virtual void Local_InitializeVFX() override;
};
