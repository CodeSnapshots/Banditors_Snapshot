// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Components/VerticalBox.h"

#include "Core/TRMacros.h"
#include "UI/TRWidget.h"
#include "UI/BossStateWidget.h"
#include "BossfightWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UBossfightWidget : public UTRWidget
{
	GENERATED_BODY()

public:
	UBossfightWidget();
	
public:
	// 주어진 목록들로 보스 정보를 갱신한다
	// 기본적으로 초기화 용도로 사용되며, 도중에 보스 정보에 변경이 있는 경우에도 (추가/제거) 호출이 가능하다
	// 그러나 이 함수는 모든 보스에 대해 업데이트를 진행하기 때문에 무겁고,
	// 고로 대부분의 게임플레이 케이스(e.g. 보스A가 데미지를 입음)에는 이 함수 대신 BossStateWidget의 Update가 호출되어야 한다
	void Update(const TArray<class AGameCharacter*>& Bosses);

public:
	UPROPERTY()
	TSubclassOf<UUserWidget> BossStateWidgetClass = nullptr;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* BossStateVertBox = nullptr;

	// 보스 몬스터와 해당 몬스터의 정보를 나타내는 위젯을 맵핑
	TMap<TWeakObjectPtr<class AGameCharacter>, TWeakObjectPtr<UBossStateWidget>> WidgetMap;
};
