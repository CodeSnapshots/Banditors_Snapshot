// Copyright (C) 2025 by Haguk Kim


#include "UI/StatusEffectListWidget.h"
#include "Core/TRMacros.h"
#include "UI/StatusEffectWidget.h"
#include "StatusEffect/StatusEffect.h"
#include "Characters/GameCharacter.h"

UStatusEffectListWidget::UStatusEffectListWidget()
{
	static ConstructorHelpers::FClassFinder <UUserWidget> StatEffWidgetFinder(TEXT(ASSET_DEFAULT_STATEFF_WIDGET));
	if (StatEffWidgetFinder.Succeeded())
	{
		SEWidgetClass = StatEffWidgetFinder.Class;
	}
	if (!SEWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UStatusEffectListWidget - StatusEffect widget class not found! - path: %s"), *FString(ASSET_DEFAULT_STATEFF_WIDGET));
	}
}

void UStatusEffectListWidget::AddStatusEffect(UStatusEffect* EffectInst)
{
	if (!EffectVertBox || !EffectInst) return;
	UStatusEffectWidget* SEWidget = CreateWidget<UStatusEffectWidget>(this, SEWidgetClass);
	if (SEWidget)
	{
		SEWidget->BindWithStatusEffect(EffectInst);

		EffectVertBox->AddChild(SEWidget);
		SEInstances.Add(EffectInst, SEWidget);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStatusEffectListWidget::AddStatusEffect - Status effect widget creation failed!"));
	}
}

void UStatusEffectListWidget::Update(AGameCharacter* Target)
{
	if (!IsValid(Target))
	{
		for (UWidget* Child : EffectVertBox->GetAllChildren())
		{
			EffectVertBox->RemoveChild(Child);
		}
		return;
	}
	
	// 추가
	const TArray<UStatusEffect*>& TargetEffects = Target->GetAppliedStatEffects();
	for (UStatusEffect* TargetEffect : TargetEffects)
	{
		if (!TargetEffect || !TargetEffect->ShouldDisplayUI()) continue;
		if (SEInstances.Contains(TargetEffect)) continue;
		AddStatusEffect(TargetEffect);
	}

	// 제거
	TMap<TWeakObjectPtr<UStatusEffect>, UStatusEffectWidget*> CopiedSEInstances = SEInstances;
	for (const TPair<TWeakObjectPtr<UStatusEffect>, UStatusEffectWidget*>& Pair : CopiedSEInstances)
	{
		if (!TargetEffects.Contains(Pair.Key) || !Pair.Key.IsValid())
		{
			if (Pair.Value)
			{
				Pair.Value->RemoveFromParent();
				EffectVertBox->RemoveChild(Cast<UWidget>(Pair.Value));
			}
			SEInstances.Remove(Pair.Key);
		}
	}
}
