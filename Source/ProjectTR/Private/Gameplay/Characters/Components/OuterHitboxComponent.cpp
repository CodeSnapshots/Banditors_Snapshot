// Copyright (C) 2024-2025 by Haguk Kim


#include "Characters/Components/OuterHitboxComponent.h"
#include "Core/TRMacros.h"
#include "Characters/GameCharacter.h"

UOuterHitboxComponent::UOuterHitboxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionProfileName("OuterHitbox");
	SetGenerateOverlapEvents(false);
	SetShouldUpdatePhysicsVolume(false);
	SetSimulatePhysics(false);
}

void UOuterHitboxComponent::OnOuterHitboxCollision(float HitboxDuration)
{
	AGameCharacter* HitboxOwner = Cast<AGameCharacter>(GetOwner());
	if (HitboxOwner)
	{
		if (HitboxDuration <= UE_SMALL_NUMBER)
		{
			HitboxOwner->ActivateDetailHitboxForTick();
		}
		else
		{
			HitboxOwner->ActivateDetailedHitboxFor(HitboxDuration);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OnOuterHitboxCollision - Hitbox has no valid owner!"));
	}
}
