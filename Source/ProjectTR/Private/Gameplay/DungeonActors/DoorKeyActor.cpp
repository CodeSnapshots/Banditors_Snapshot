// Copyright (C) 2025 by Haguk Kim


#include "DungeonActors/DoorKeyActor.h"
#include "EngineUtils.h"

#include "Core/TRGameState.h"
#include "Core/TRMacros.h"
#include "Core/TRCVar.h"
#include "DungeonActors/LockedDoorActor.h"
#include "Characters/FPSCharacter.h"

void ADoorKeyActor::OnMuzzleTriggered(AGameCharacter* TriggeredBy)
{
	UWorld* World = GetWorld();
	AFPSCharacter* TriggerChar = Cast<AFPSCharacter>(TriggeredBy);
	if (World && TriggerChar)
	{
		ATRGameState* TRGS = World->GetGameState<ATRGameState>();
		if (TRGS)
		{
#if WITH_EDITOR
			if (CVarShowScreenDebugMsgs.GetValueOnGameThread())
			{
				TR_PRINT_ARGS("Key obtained: %d", KeyId);
			}
#endif

			TRGS->Server_AddDoorKey(KeyId);
			bCanBeDestroyed = true;
			Destroy();
		}
	}
}

void ADoorKeyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bCanBeDestroyed) return;

	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<ALockedDoorActor> It(World); It; ++It)
		{
			ALockedDoorActor* LockedDoor = *It;
			if (LockedDoor->GetDoorId() == GetKeyId())
			{
				UE_LOG(LogTemp, Error, TEXT("ADoorKeyActor - Unexpected key destruction! Force-opening matching locked door(s) to prevent permanent locked door(s)!"));
				LockedDoor->Unlock();
			}
		}
	}
	return;
}
