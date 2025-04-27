// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "TRWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AllyStateWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UAllyStateWidget : public UTRWidget
{
	GENERATED_BODY()

public:
	void Bind(class AFPSCharacter* Ally);

	// 사전에 바인딩 된 캐릭터 및 플레이어에 대해 업데이트한다
	UFUNCTION()
	void Update();

	// Null이 아닌 무언가에 바인딩 되어있는지 여부
	bool IsBoundedToSomething() { return !BoundPlayer.IsExplicitlyNull(); }

protected:
	// 바인딩된 플레이어 캐릭터
	TWeakObjectPtr<class AFPSCharacter> BoundPlayer = nullptr;

	// 플레이어 캐릭터 체력바
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* WB_HealthProgressBar = nullptr;

	// 플레이어 이름
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerNameText;

	// 플레이어 레벨
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerLevelText;
};
