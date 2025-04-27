// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Components/VerticalBox.h"

#include "UI/TRWidget.h"
#include "StatusEffectListWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UStatusEffectListWidget : public UTRWidget
{
	GENERATED_BODY()

public:
	UStatusEffectListWidget();

public:
	// 상태이상에 맞는 위젯을 생성해 추가한다
	void AddStatusEffect(class UStatusEffect* EffectInst);

	// Invalid한 상태이상이 있는지 확인하고 있다면 연결된 위젯들을 제거한다
	void Update(class AGameCharacter* Target);
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* EffectVertBox = nullptr;

	// 생성에 사용할 위젯 블루프린트 클래스
	UPROPERTY()
	TSubclassOf<UUserWidget> SEWidgetClass = nullptr;

	// 현재 참조중인 상태이상 인스턴스들과 각각에 대응되는 위젯 목록
	// 상태이상 인스턴스가 파괴되었을 경우 대응되는 위젯도 제거한다
	TMap<TWeakObjectPtr<class UStatusEffect>, class UStatusEffectWidget*> SEInstances;
};
