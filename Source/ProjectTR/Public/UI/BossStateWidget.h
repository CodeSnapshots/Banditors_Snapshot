// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"

#include "UI/TRWidget.h"
#include "UI/TRProgressBar.h"
#include "BossStateWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UBossStateWidget : public UTRWidget
{
	GENERATED_BODY()
	
public:
	void Bind(class AGameCharacter* Boss);

	// 사전에 바인딩 된 보스에 대해 업데이트한다
	UFUNCTION()
	void Update();

	// Null이 아닌 무언가에 바인딩 되어있는지 여부
	bool IsBoundedToSomething() { return !BoundBoss.IsExplicitlyNull(); }

protected:
	// 바인딩된 보스 캐릭터
	TWeakObjectPtr<class AGameCharacter> BoundBoss = nullptr;

	// 보스 체력바
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTRProgressBar* WB_TRProgressBar = nullptr;
};
