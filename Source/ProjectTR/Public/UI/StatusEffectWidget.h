// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "UI/TRWidget.h"
#include "StatusEffectWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UStatusEffectWidget : public UTRWidget
{
	GENERATED_BODY()

public:
	// 상태이상과 바인딩함과 동시에 이 위젯의 세부 값들을 해당 상태이상에 맞게 초기화한다
	UFUNCTION()
	void BindWithStatusEffect(class UStatusEffect* Effect);
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* EffectIcon = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* EffectNameText = nullptr;

protected:
	TWeakObjectPtr<class UStatusEffect> BoundEffect = nullptr;
};
