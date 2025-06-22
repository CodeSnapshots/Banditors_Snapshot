// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/DungeonActors/BreakableActor.h"
#include "DungeonCrate.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ADungeonCrate : public ABreakableActor
{
	GENERATED_BODY()
	
public:
	ADungeonCrate();
	
	// 로직 오버라이드
	virtual void Server_ProcessCrumbling(const FVector& CrumblingVelocity) override;
	virtual bool CanBreak(AActor* CollidedActor) override;
};
