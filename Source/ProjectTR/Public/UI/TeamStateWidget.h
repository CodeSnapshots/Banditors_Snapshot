// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Components/VerticalBox.h"

#include "Core/TRMacros.h"
#include "UI/AllyStateWidget.h"
#include "UI/TRWidget.h"
#include "TeamStateWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UTeamStateWidget : public UTRWidget
{
	GENERATED_BODY()

public:
	UTeamStateWidget();
	void Local_Init();

public:
	// 주어진 목록들로 아군 정보를 갱신한다
	// 기본적으로 초기화 용도로 사용되며, 도중에 팀원 정보에 변경이 있는 경우에도 (접속/접속 해제) 호출이 가능하다
	// 그러나 이 함수는 모든 팀원들에 대해 업데이트를 진행하기 때문에 무겁고,
	// 고로 대부분의 게임플레이 케이스에는 이 함수 대신 AllyStateWidget의 Update가 호출되어야 한다
	void Update(const TArray<class AFPSCharacter*>& Allies);

public:
	UPROPERTY()
	TSubclassOf<UUserWidget> AllyStateWidgetClass = nullptr;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* AllyStateVertBox = nullptr;

	// 아군과 해당 아군 캐릭터의 정보를 나타내는 위젯을 맵핑
	TMap<TWeakObjectPtr<class AFPSCharacter>, TWeakObjectPtr<UAllyStateWidget>> WidgetMap;
};
