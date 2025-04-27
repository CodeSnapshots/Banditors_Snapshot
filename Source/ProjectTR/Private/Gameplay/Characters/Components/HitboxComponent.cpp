// Copyright (C) 2024-2025 by Haguk Kim


#include "Characters/Components/HitboxComponent.h"
#include "Core/TRMacros.h"

UHitboxComponent::UHitboxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetComponentTickEnabled(false);

	SetCollisionProfileName("Hitbox");
	SetGenerateOverlapEvents(false);
	SetSimulatePhysics(false);
	SetShouldUpdatePhysicsVolume(false);
}