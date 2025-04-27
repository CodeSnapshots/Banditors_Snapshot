// Copyright (C) 2025 by Haguk Kim


#include "UI/BossfightWidget.h"
#include "Characters/GameCharacter.h"

UBossfightWidget::UBossfightWidget()
{
	static ConstructorHelpers::FClassFinder <UUserWidget> BossStateWidgetFinder(TEXT(ASSET_DEFAULT_BOSSSTATE_WIDGET));
	if (BossStateWidgetFinder.Succeeded())
	{
		BossStateWidgetClass = BossStateWidgetFinder.Class;
	}
	if (!BossStateWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UBossfightWidget - Boss state widget class not found! - path: %s"), *FString(ASSET_DEFAULT_BOSSSTATE_WIDGET));
	}
}

void UBossfightWidget::Update(const TArray<AGameCharacter*>& Bosses)
{
	for (AGameCharacter* Boss : Bosses)
	{
		if (!IsValid(Boss)) continue;

		if (!WidgetMap.Contains(Boss))
		{
			UBossStateWidget* BossStateWidget = CreateWidget<UBossStateWidget>(this, BossStateWidgetClass);
			if (BossStateWidget)
			{
				BossStateWidget->Bind(Boss);
				WidgetMap.Add(Boss, BossStateWidget);
				BossStateVertBox->AddChild(BossStateWidget);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UBossfightWidget::Update - Boss state widget creation failed! Aborting!"));
				continue;
			}
		}

		TWeakObjectPtr<UBossStateWidget> WeakBossStateWidget = WidgetMap[Boss];
		if (WeakBossStateWidget.IsValid())
		{
			WeakBossStateWidget->Update();
		}
	}

	// 캐릭터가 제거(파괴 혹은 리스트에서 삭제)되었을 경우 위젯도 제거해야 함
	for (auto It = WidgetMap.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid() || !Bosses.Contains(It.Key()))
		{
			if (It.Value().IsValid())
			{
				It.Value()->RemoveFromParent();
				It.Value()->RemoveFromRoot();
			}
			It.RemoveCurrent();
		}
	}
}
