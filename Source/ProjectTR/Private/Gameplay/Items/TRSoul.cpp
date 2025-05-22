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

bool ATRSoul::RestoreFromItemData(UItemData* Data)
{
	if (!Super::RestoreFromItemData(Data)) return false;
	USoulItemData* SoulData = Cast<USoulItemData>(Data);
	if (!IsValid(SoulData))
	{
		UE_LOG(LogTemp, Error, TEXT("Tried to restore a soul item from a non-SoulItemData %s."), *(Data->GetName()));
		return false;
	}
	if (!HasAuthority())
	{
		// 클라이언트는 아무 것도 복구하지 않음
		// 만약 클라에 복구해야 할 값이 생길 경우 현재 코드 블록에 진입하기 이전에 처리를 완료하게 작성하면 됨
		UE_LOG(LogTemp, Warning, TEXT("Client does not have anything to restore from soul item data. This is a normal behaviour."), *(Data->GetName()));
		return true;
	}
	if (!IsValid(SoulData->GetCachedController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has invalid cached data! Aborting."), *(Data->GetName()));
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


