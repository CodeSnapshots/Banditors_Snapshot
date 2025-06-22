// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/TRSoul.h"
#include "Core/TRPlayerController.h"
#include "Items/SoulItemData.h"
#include "Characters/FPSCharacter.h"

ATRSoul::ATRSoul()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent->SetGenerateOverlapEvents(true); // 제단
}

void ATRSoul::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ATRSoul::Server_RestoreItem_PreSpawn(const UInvObject* SrcInvObject)
{
	if (!Super::Server_RestoreItem_PreSpawn(SrcInvObject)) return false;
	const USoulItemData* SoulData = Cast<USoulItemData>(SrcInvObject->GetItemData());
	if (!IsValid(SoulData))
	{
		UE_LOG(LogTemp, Error, TEXT("Tried to restore a soul item from a non-SoulItemData InvObj %s."), *(SrcInvObject->GetName()));
		return false;
	}
	if (!IsValid(SoulData->GetCachedController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has invalid cached data! Aborting."), *(SrcInvObject->GetName()));
		return false;
	}

	// 복구 로직
	if (SoulData->GetCachedController())
	{
		Server_SetController(SoulData->GetCachedController());
	}
	return true;
}

bool ATRSoul::IsReadyToRespawnPlayer() const
{
	// TODO: 소울이 특정인이 아닌 아무 사람이나 살릴 수 있게 하기
	if (!IsValid(Server_Controller))
	{
		UE_LOG(LogTemp, Error, TEXT("IsReadyToRespawnPlayer - Lacking necessary data! %d"), (Server_Controller != nullptr));
		return false;
	}

	AFPSCharacter* FPSPawn = Cast<AFPSCharacter>(Server_Controller->GetPawn());
	if (FPSPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("IsReadyToRespawnPlayer - Controller already owns a FPSCharacter!"));
		return false;
	}

	return true;
}


