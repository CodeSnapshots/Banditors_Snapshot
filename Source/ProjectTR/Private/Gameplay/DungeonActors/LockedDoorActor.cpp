// Copyright (C) 2025 by Haguk Kim


#include "DungeonActors/LockedDoorActor.h"
#include "Core/TRGameState.h"
#include "Core/TRCVar.h"
#include "Characters/FPSCharacter.h"

void ALockedDoorActor::OnMuzzleTriggered(AGameCharacter* TriggeredBy)
{
	// 부모 로직 완전히 재정의
	UWorld* World = GetWorld();
	AFPSCharacter* TriggerChar = Cast<AFPSCharacter>(TriggeredBy);
	if (World && TriggerChar)
	{
		ATRGameState* TRGS = World->GetGameState<ATRGameState>();
		if (TRGS)
		{
			// 범용 열쇠로 개방이 허용되는 모든 문들을 열 수 있다
			if (TRGS->Server_UseDoorKeySameOrLessThan(TR_MAX_MULTIKEY_LOCKED_DOORS_PER_LEVEL /*공용 키 ID의 최댓값은 공용키로 잠긴방 개수의 최댓값을 넘을 수 없음*/))
			{
				Unlock();
				TriggerThis(); // 문 열기
			}
		}
	}
}

void ALockedDoorActor::Unlock()
{
	if (!bIsLocked) return;

#if WITH_EDITOR
	if (CVarShowScreenDebugMsgs.GetValueOnGameThread())
	{
		TR_PRINT_ARGS("Door unlocked: %d", DoorId);
	}
#endif
	bIsLocked = false;
}
