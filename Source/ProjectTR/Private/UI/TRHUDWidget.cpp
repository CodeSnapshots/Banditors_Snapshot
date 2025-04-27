// Copyright (C) 2025 by Haguk Kim


#include "UI/TRHUDWidget.h"
#include "Core/TRPlayerController.h"
#include "Core/TRGameState.h"
#include "Characters/GameCharacter.h"
#include "Characters/FPSCharacter.h"
#include "Inventory/EquipSystem.h"
#include "Items/WieldItem.h"

void UTRHUDWidget::SetTarget(AGameCharacter* Target)
{
	if (HudTarget)
	{
		HudTarget->UnbindHUD();
		HudTarget = nullptr;
	}
	Target->BindHUD(this);
	HudTarget = Target;
	UpdateAll();
}

void UTRHUDWidget::UpdateAmmo(int32 ClientAmmoPrediction)
{
	if (!AmmoLeftText) return;

	AFPSCharacter* HudFPSTarget = Cast<AFPSCharacter>(HudTarget);
	if (!IsValid(HudFPSTarget))
	{
		AmmoLeftText->SetText(FText::FromString(""));
	}
	else
	{
		if (ClientAmmoPrediction < 0)
		{
			AmmoLeftText->SetText(FText::FromString(HudFPSTarget->Host_GetCurrWeaponAmmoLeft()));
		}
		else
		{
			AmmoLeftText->SetText(FText::FromString(FString::Printf(TEXT("%d"), ClientAmmoPrediction)));
		}
	}
}

void UTRHUDWidget::UpdateSlot()
{
	if (!SlotText) return;

	if (!IsValid(HudTarget) || !HudTarget->EquipSystem)
	{
		SlotText->SetText(FText::FromString(""));
	}
	else
	{
		AWieldItem* SlotItem = HudTarget->EquipSystem->GetCurrWeaponActor();
		if (!IsValid(SlotItem))
		{
			SlotText->SetText(FText::FromString(""));
		}
		else
		{
			SlotText->SetText(FText::FromString(FString::Printf(TEXT("%s"), *SlotItem->GetInvObject()->GetInvObjName())));
		}
	}
}

void UTRHUDWidget::UpdateHealthBar()
{
	if (!WB_HealthBar) return;
	WB_HealthBar->Update(HudTarget);
}

void UTRHUDWidget::UpdateExp()
{
	if (!WB_LevelExpBar) return;
	WB_LevelExpBar->Update(HudTarget);
}

void UTRHUDWidget::UpdateShards()
{
	if (!WB_Shards) return;
	WB_Shards->Update(HudTarget);
}

void UTRHUDWidget::UpdateStatusEffects()
{
	if (!WB_StatusEffectList) return;
	WB_StatusEffectList->Update(HudTarget);
}

void UTRHUDWidget::UpdateAll()
{
	UpdateAmmo();
	UpdateSlot();
	UpdateHealthBar();
	UpdateExp();
	UpdateShards();
	UpdateStatusEffects();
}

