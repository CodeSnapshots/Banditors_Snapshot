// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/SystemActors/CharacterTriggerVolume.h"
#include "JumpPad.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API AJumpPad : public ACharacterTriggerVolume
{
	GENERATED_BODY()
	
public:
	AJumpPad();

protected:
	virtual void ProcessCharacterOverlap(class AGameCharacter* Target) override;

public:
	UPROPERTY(EditAnywhere)
	float JumpSpeed = 1000.0f;

	UPROPERTY(EditAnywhere)
	bool bOverrideXYVelocity = false;

	UPROPERTY(EditAnywhere)
	bool bOverrideZVelocity = true;
};
