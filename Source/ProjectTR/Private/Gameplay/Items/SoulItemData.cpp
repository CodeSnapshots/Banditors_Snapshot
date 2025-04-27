// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/SoulItemData.h"
#include "Core/TRStructs.h"
#include "Core/TRPlayerController.h"
#include "Items/TRSoul.h"
#include "Characters/FPSCharacter.h"

USoulItemData::USoulItemData()
{

}

bool USoulItemData::CacheItem(const ABaseItem* Item)
{
	if (!Super::CacheItem(Item)) return false;

	// NOTE: Soul에 저장되는 정보는 전부 Server authoritative함
	const ATRSoul* SoulItem = Cast<ATRSoul>(Item);
	if (!IsValid(SoulItem))
	{
		UE_LOG(LogTemp, Error, TEXT("Tried to cache a non-SoulItem %s to a SoulItemData."), *(Item->GetName()));
		return false;
	}

	if (SoulItem->Server_GetCharacterClass())
	{
		CacheCharacterClass(SoulItem->Server_GetCharacterClass());
		CacheController(SoulItem->Server_GetController());
		CacheInstanceData(SoulItem->Server_GetInstanceData());
	}
	return true;
}
