// Copyright (C) 2024-2025 by Haguk Kim


#include "SystemActors/LobbyFallVolume.h"
#include "GameFramework/GameModeBase.h"

#include "Characters/FPSCharacter.h"

void ALobbyFallVolume::ProcessPlayerOverlap(AFPSCharacter* Player, bool bAllPlayerOverlapped)
{
	if (!HasAuthority()) return;
	if (!Player || !Player->GetController()) return;

	FVector SpawnLocation;
	AActor* PlayerStartActor = GetWorld()->GetAuthGameMode()->FindPlayerStart(Player->GetController());
	Player->SetActorLocation(PlayerStartActor->GetActorLocation());
}
