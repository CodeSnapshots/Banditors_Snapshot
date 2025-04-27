// Copyright (C) 2024-2025 by Haguk Kim


#include "DungeonActors/TouchActivationTrap.h"

ATouchActivationTrap::ATouchActivationTrap()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	check(MeshComponent);
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetShouldUpdatePhysicsVolume(false);
}

