// Copyright (C) 2024-2025 by Haguk Kim


#include "Core/TRGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Blueprint/DragDropOperation.h"
#include "GameFramework/PlayerState.h"

#include "Core/TRPlayerController.h"
#include "Core/ProjectTRGameModeBase.h"
#include "Core/TRGameState.h"
#include "Core/TRMacros.h"
#include "Core/SoundSubsystem.h"
#include "Characters/GameCharacter.h"

UTRGameInstance::UTRGameInstance()
{
	OnNotifyPreClientTravel().AddUObject(this, &UTRGameInstance::Local_OnLevelTransitionBegin);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UTRGameInstance::Local_OnLevelTransitionComplete);
}

USoundSubsystem* UTRGameInstance::GetSoundManager()
{
	return GetSubsystem<USoundSubsystem>();
}

void UTRGameInstance::Server_OnDescendingDungeon(int32 NewDepth)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("Server_OnDescendingDungeon - GameInstance has no world set! Aborting."));
		return;
	}
	if (!GetWorld()->GetAuthGameMode())
	{
		UE_LOG(LogTemp, Error, TEXT("Server_OnDescendingDungeon - Client should never call this function!"));
		return;
	}
	if (NewDepth < 0)
	{
		// 음수는 층계를 변환하지 않음을 의미
		return;
	}
	Server_ChangeDungeonDepthTo(NewDepth);
}

void UTRGameInstance::Server_ChangeDungeonDepthTo(int32 TargetDepth)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("Server_ChangeDungeonDepth - GameInstance has no world set! Aborting."));
		return;
	}
	if (!GetWorld()->GetAuthGameMode())
	{
		UE_LOG(LogTemp, Error, TEXT("Server_ChangeDungeonDepth - Client should never call this function!"));
		return;
	}
	if (TargetDepth < Server_CurrDungeonDepth)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_ChangeDungeonDepth - Ascending the dungeon is not intended in current gameplay design! Please check."));
	}
	else if (TargetDepth == Server_CurrDungeonDepth)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_ChangeDungeonDepth - TargetDepth is same as Server_CurrDungeonDepth. This might be unintentional."));
	}
	Server_CurrDungeonDepth = TargetDepth;
}

void UTRGameInstance::Local_OnLevelTransitionBegin(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	if (bWaitingForLevelTransitionComplete) return;
	bWaitingForLevelTransitionComplete = true;
	UE_LOG(LogTemp, Warning, TEXT("Local_OnLevelTransitionBegin - Auth: %d"), !GetWorld()->IsNetMode(ENetMode::NM_Client));

	// 컨트롤 폰과 바인딩된 위젯 제거
	ATRPlayerController* LocalController = Cast<ATRPlayerController>(GetFirstLocalPlayerController());
	if (LocalController)
	{
		LocalController->Local_DerefPawnBoundedWidgets(LocalController->GetPawn());
	}

	// 게임 스테이트와 바인딩된 위젯 제거
	UWorld* World = GetWorld(); // 아직은 월드가 유효하며, 기존 월드를 가리킴
	if (World)
	{
		ATRGameState* LocalGameState = World->GetGameState<ATRGameState>();
		if (LocalGameState && LocalGameState->GameStateHUD)
		{
			LocalGameState->GameStateHUD->RemoveFromParent();
			LocalGameState->GameStateHUD->RemoveFromRoot();
		}
	}

	// 드래그 드랍 액션 전부 취소; 이는 페이로드에 게임 로직 오브젝트가 남아있을 경우 잘못된 메모리를 참조하게 되기 때문임
	// 이는 크래시 혹은 메모리 누수로 이어질 수 있음
	Local_CancelAllDragDrops();
}

void UTRGameInstance::Local_OnLevelTransitionComplete(UWorld* NewWorld)
{
	if (!NewWorld) return;
	if (!bWaitingForLevelTransitionComplete) return;
	bWaitingForLevelTransitionComplete = false;
	UE_LOG(LogTemp, Warning, TEXT("Local_OnLevelTransitionComplete - Auth: %d"), !NewWorld->IsNetMode(ENetMode::NM_Client));
}

void UTRGameInstance::Local_CancelAllDragDrops()
{
	if (FSlateApplication::Get().IsDragDropping())
	{
		FSlateApplication::Get().CancelDragDrop();
	}
}
