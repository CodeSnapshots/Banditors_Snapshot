// Copyright (C) 2025 by Haguk Kim


#include "UI/TeamStateWidget.h"
#include "EngineUtils.h"

#include "Characters/FPSCharacter.h"

UTeamStateWidget::UTeamStateWidget()
{
	static ConstructorHelpers::FClassFinder <UUserWidget> AllyStateWidgetFinder(TEXT(ASSET_DEFAULT_ALLYSTATE_WIDGET));
	if (AllyStateWidgetFinder.Succeeded())
	{
		AllyStateWidgetClass = AllyStateWidgetFinder.Class;
	}
	if (!AllyStateWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UTeamStateWidget - Ally state widget class not found! - path: %s"), *FString(ASSET_DEFAULT_ALLYSTATE_WIDGET));
	}
}

void UTeamStateWidget::Local_Init()
{
	if (!GetWorld()) return;
	TArray<AFPSCharacter*> Allies;
	for (TActorIterator<AFPSCharacter> It(GetWorld()); It; ++It)
	{
		// 사망 정보는 레플리케이션 되므로 클라 접근 가능
		// 로컬 폰 제외
		if (*It && !((*It)->GetHasDied()) && !(*It)->IsLocallyControlled())
		{
			Allies.Add(*It);
		}
	}
	Update(Allies);
}

void UTeamStateWidget::Update(const TArray<class AFPSCharacter*>& Allies)
{
	for (AFPSCharacter* Ally : Allies)
	{
		if (!IsValid(Ally)) continue;

		if (!WidgetMap.Contains(Ally))
		{
			UAllyStateWidget* AllyStateWidget = CreateWidget<UAllyStateWidget>(this, AllyStateWidgetClass);
			if (AllyStateWidget)
			{
				AllyStateWidget->Bind(Ally);
				WidgetMap.Add(Ally, AllyStateWidget);
				AllyStateVertBox->AddChild(AllyStateWidget);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UTeamStateWidget::Update - Ally state widget creation failed! Aborting!"));
				continue;
			}
		}

		TWeakObjectPtr<UAllyStateWidget> WeakAllyStateWidget = WidgetMap[Ally];
		if (WeakAllyStateWidget.IsValid())
		{
			WeakAllyStateWidget->Update();
		}
	}

	// 캐릭터가 제거(파괴 혹은 리스트에서 삭제)되었을 경우 위젯도 제거해야 함
	for (auto It = WidgetMap.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid() || !Allies.Contains(It.Key()))
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
