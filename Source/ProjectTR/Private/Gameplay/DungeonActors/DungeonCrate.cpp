// Copyright (C) 2025 by Haguk Kim


#include "Gameplay/DungeonActors/DungeonCrate.h"

ADungeonCrate::ADungeonCrate()
{
}

void ADungeonCrate::Server_ProcessCrumbling(const FVector& CrumblingVelocity)
{
	return Super::Server_ProcessCrumbling(CrumblingVelocity);
}

bool ADungeonCrate::CanBreak(AActor* CollidedActor)
{
	return Super::CanBreak(CollidedActor);
}
