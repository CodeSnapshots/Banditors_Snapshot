// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "SystemActors/CharacterTriggerVolume.h"
#include "DescendVolume.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ADescendVolume : public ACharacterTriggerVolume
{
	GENERATED_BODY()
	
protected:
	virtual void ProcessPlayerOverlap(class AFPSCharacter* Player, bool bAllPlayersOverlapped) override;
	virtual void ProcessAllPlayersOverlap() override;
};
