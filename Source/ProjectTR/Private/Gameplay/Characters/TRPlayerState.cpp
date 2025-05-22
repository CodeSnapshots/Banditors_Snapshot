// Copyright (C) 2024-2025 by Haguk Kim


#include "Characters/TRPlayerState.h"
#include "Core/ProjectTRGameModeBase.h"
#include "Core/TRCVar.h"
#include "Characters/GameCharacter.h"
#include "Spectation/TRSpectatorPawn.h"

void ATRPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (!HasAuthority()) return;
	if (ATRPlayerState* NewPlayerState = Cast<ATRPlayerState>(PlayerState))
	{
		NewPlayerState->bIsOut = bIsOut;

		NewPlayerState->Server_CachedPlayerInstanceData = Server_CachedPlayerInstanceData;
		NewPlayerState->bServer_IsCachedDataValid = bServer_IsCachedDataValid;
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
#if WITH_EDITOR
	if (CVarShowScreenDebugMsgs.GetValueOnGameThread())
	{
		TR_PRINT_ARGS("Caching player data of id %s", *GetUniqueId().ToString());
	}
#endif

	if (!GetPlayerController())
	{
		UE_LOG(LogTemp, Error, TEXT("Server_CachePlayerInstanceData - Invalid player controller!"));
		return;
	}

	AGameCharacter* GameCharacter = Cast<AGameCharacter>(GetPlayerController()->GetPawn());
	if (GameCharacter)
	{
		Server_CachedPlayerInstClass = GameCharacter->GetClass();

		Server_CachedPlayerInstanceData = FGameCharacterInstanceData(); // 반드시 값을 먼저 Clear해주고 새 값을 써야 함
		Server_CachedPlayerInstanceData = GameCharacter->Server_GetInstanceData();
		bServer_IsCachedDataValid = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Server_CachePlayerInstanceData - Invalid possessed pawn!"));
	}
}

TPair<TSubclassOf<AFPSCharacter>, FGameCharacterInstanceData*> ATRPlayerState::Server_GetCachedPlayerInstanceData()
{
#if WITH_EDITOR
	if (CVarShowScreenDebugMsgs.GetValueOnGameThread())
	{
		TR_PRINT_ARGS("Searching cached player data of id %s", *GetUniqueId().ToString());
	}
#endif

	TSubclassOf<AFPSCharacter> ClassResult = nullptr;
	FGameCharacterInstanceData* InstResult = nullptr;
	if (bServer_IsCachedDataValid)
	{
		ClassResult = Server_CachedPlayerInstClass;
		InstResult = &Server_CachedPlayerInstanceData;

		// 일회성 값이므로 값을 무효화해준다
		bServer_IsCachedDataValid = false;
	}
	return { ClassResult, InstResult };
}
