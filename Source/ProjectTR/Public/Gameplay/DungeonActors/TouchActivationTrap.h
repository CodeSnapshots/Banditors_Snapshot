// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "SystemActors/PlayerTriggerVolume.h"
#include "TouchActivationTrap.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ATouchActivationTrap : public APlayerTriggerVolume
{
	GENERATED_BODY()
	
public:
	ATouchActivationTrap();

public:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComponent = nullptr;
};
