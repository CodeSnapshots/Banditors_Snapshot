// Copyright (C) 2024-2025 by Haguk Kim


#include "Characters/TRPlayerState.h"
#include "Core/ProjectTRGameModeBase.h"
#include "Characters/GameCharacter.h"

void ATRPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (!HasAuthority()) return;
	if (ATRPlayerState* NewPlayerState = Cast<ATRPlayerState>(PlayerState))
	{
		NewPlayerState->bIsOut = bIsOut;

		NewPlayerState->Server_CachedPlayerInstanceData = Server_CachedPlayerInstanceData;
		NewPlayerState->bServer_IsCachedPlayerInstanceDataValid = bServer_IsCachedPlayerInstanceDataValid;
	}
}

void ATRPlayerState::OnRep_PlayerOutUpdate()
{
	// TODO: 플레이어 아웃 시 공통 로직 작성
}

void ATRPlayerState::SetIsOut(bool Value)
{
	if (!HasAuthority()) return;
	
	bIsOut = Value;
	AProjectTRGameModeBase* GameMode = Cast<AProjectTRGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->UpdateGameOverStatus();
	}
}

void ATRPlayerState::Server_CachePlayerInstanceData()
{
	AGameCharacter* GameCharacter = Cast<AGameCharacter>(GetPlayerController()->GetPawn());
	if (GameCharacter)
	{
		TR_PRINT_FSTRING("cached %s", *GetUniqueId().ToString());
		Server_CachedPlayerInstanceData = FGameCharacterInstanceData(); // 반드시 값을 먼저 Clear해주고 새 값을 써야 함
		Server_CachedPlayerInstanceData = GameCharacter->Server_GetInstanceData();
		bServer_IsCachedPlayerInstanceDataValid = true;
	}
}

FGameCharacterInstanceData* ATRPlayerState::Server_GetCachedPlayerInstanceData()
{
	FGameCharacterInstanceData* Result = nullptr;
	if (bServer_IsCachedPlayerInstanceDataValid)
	{
		TR_PRINT_FSTRING("found %s", *GetUniqueId().ToString());
		Result = &Server_CachedPlayerInstanceData;

		// 일회성 값이므로 값을 무효화해준다
		bServer_IsCachedPlayerInstanceDataValid = false;
	}
	return Result;
}
