// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TRGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UTRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UTRGameInstance();

	UFUNCTION(BlueprintCallable)
	class USoundSubsystem* GetSoundManager();

public:
	// 층계 변경 시도시 호출
	void Server_OnDescendingDungeon(int32 NewDepth);
	void Server_ChangeDungeonDepthTo(int32 TargetDepth);
	int32 Server_GetDungeonDepth() { return Server_CurrDungeonDepth; }

	// 현재 처리중인 모든 드래그 드랍 오퍼레이션을 취소한다
	void Local_CancelAllDragDrops();

protected:
	// 로컬 레벨 트랜지션 로직
	// 서버 및 클라이언트 모두 호출됨
	// NOTE: 이 함수들은 상황에 따라 단일 레벨 트랜지션임에도 여러번 호출될 수 있다는 점 유의
	UFUNCTION()
	void Local_OnLevelTransitionBegin(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel);

	UFUNCTION()
	void Local_OnLevelTransitionComplete(UWorld* NewWorld);

private:
	// Local_OnLevelTransitionBegin, Local_OnLevelTransitionComplete 호출의 1대1 맵핑을 보장하기 위한 플래그
	bool bWaitingForLevelTransitionComplete = false;

protected:
	// 던전 깊이 (2는 지하 2층을 의미)
	// NOTE: 이 값은 최초 던전을 생성할 때에만 사용되며, 
	// 게임플레이 상에서 던전 깊이를 구하기 위해선 게임모드의 변수를 사용할 것
	int32 Server_CurrDungeonDepth = 0;
};
