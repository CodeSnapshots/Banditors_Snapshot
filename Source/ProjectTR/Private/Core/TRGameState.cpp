// Copyright (C) 2024-2025 by Haguk Kim


#include "Core/TRGameState.h"
#include "EngineUtils.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

#include "Core/TRUtils.h"
#include "Core/TRMacros.h"
#include "Core/TRPlayerController.h"
#include "Characters/GameCharacter.h"
#include "Characters/BotCharacter.h"
#include "UI/TRHUDWidget.h"
#include "UI/TRGameStateHUD.h"
#include "UI/DamageNumberWidget.h"

void ATRGameState::BeginPlay()
{
	Super::BeginPlay();

	GameStateHUD = CreateWidget<UTRGameStateHUD>(GetWorld(), DefaultGameStateHUDClass);
	if (!GameStateHUD)
	{
		UE_LOG(LogTemp, Error, TEXT("ATRGameState::BeginPlay - GameStateHUD class is not created!"));
	}
	else
	{
		GameStateHUD->SetTarget(this);
		GameStateHUD->AddToViewport(WZO_HUD);
	}
}

void ATRGameState::Server_SetDungeonTimeLeft(int32 Value)
{
	if (!HasAuthority()) return;
	if (Value == DungeonTimeLeft) return;
	DungeonTimeLeft = Value;
	
	// 서버의 경우 수동 호출
	Local_OnDungeonTimeLeftUpdated();
}

FString ATRGameState::GetDungeonTimeString()
{
    return TRUtils::TimeSecondsToString(DungeonTimeLeft);
}

void ATRGameState::Server_ProcessRedModeEnter()
{
	if (!HasAuthority()) return;
	Multicast_ProcessRedModeEnter();
}

void ATRGameState::Multicast_ProcessRedModeEnter_Implementation()
{
	// 레드모드 진입 시 라이트 효과 부여
	FLinearColor Red = FLinearColor(255, 0, 0);
	Local_SetAllLightSettings<APointLight>(1.0f, "RedMode", &Red);
	Local_SetAllLightSettings<ADirectionalLight>(1.0f, "RedMode", &Red);
	// NOTE: Skylight는 제외한다
}

void ATRGameState::Server_AddDoorKey(int32 KeyId)
{
	if (DungeonDoorKeys.Contains(KeyId))
	{
		UE_LOG(LogTemp, Error, TEXT("Server_AddDoorKey - Key id %d already exists! Aborting."), KeyId);
		return;
	}
	DungeonDoorKeys.Add(KeyId);

	// 서버의 경우 직접 호출한다
	Local_OnDoorKeysChanged();
}

bool ATRGameState::Server_UseDoorKey(int32 KeyId)
{
	bool bKeyRemoved = DungeonDoorKeys.Remove(KeyId) > 0;
	if (bKeyRemoved)
	{
		// 서버의 경우 직접 호출한다
		Local_OnDoorKeysChanged();
	}
	return bKeyRemoved;
}

bool ATRGameState::Server_UseDoorKeySameOrLessThan(int32 KeyId)
{
	int32 Idx = -1;
	for (int32 i = 0; i < DungeonDoorKeys.Num(); ++i)
	{
		if (DungeonDoorKeys[i] <= KeyId)
		{
			Idx = i;
			break;
		}
	}
	if (Idx >= 0)
	{
		DungeonDoorKeys.RemoveAt(Idx);

		// 서버의 경우 직접 호출한다
		Local_OnDoorKeysChanged();
	}
	return Idx >= 0;
}

void ATRGameState::Server_AddBossCharacter(ABotCharacter* NewBoss)
{
	if (!IsValid(NewBoss))
	{
		UE_LOG(LogTemp, Error, TEXT("Server_AddBossCharacter - Trying to add invalid character! Aborting!"));
		return;
	}
	BossCharacters.Add(NewBoss);
	// 서버의 경우 직접 호출해주어야 한다
	Local_UpdateBossState();
}

bool ATRGameState::Server_RemoveBossCharacter(ABotCharacter* OldBoss)
{
	if (!IsValid(OldBoss))
	{
		UE_LOG(LogTemp, Error, TEXT("Server_AddBossCharacter - Trying to remove invalid character! Aborting!"));
		return false;
	}
	BossCharacters.Remove(OldBoss);
	// 서버의 경우 직접 호출해주어야 한다
	Local_UpdateBossState();
	return true;
}

const TArray<class AGameCharacter*>& ATRGameState::Local_GetBossCharacters()
{
	return BossCharacters;
}

void ATRGameState::OnRep_DungeonTimeLeft()
{
	Local_OnDungeonTimeLeftUpdated();
}

void ATRGameState::Local_OnDungeonTimeLeftUpdated()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// UI 업데이트 (서버,클라)
	if (!GameStateHUD) return;
	ATRPlayerController* TRPC = World->GetFirstLocalPlayerFromController<ATRPlayerController>();
	if (TRPC)
	{
		AGameCharacter* GameChar = Cast<AGameCharacter>(TRPC->GetCharacter());
		if (GameChar && GameChar->Local_GetBoundHUDWidget().IsValid())
		{
			GameStateHUD->UpdateDungeonTimer();
		}
	}
}

void ATRGameState::OnRep_BossCharacters()
{
	Local_UpdateBossState();
}

void ATRGameState::Local_UpdateBossState()
{
	if (!IsValid(GameStateHUD)) return;
	GameStateHUD->UpdateBossfight();
}

ALocalDamageNumber* ATRGameState::Local_DisplayDamageNumber(UWorld* World, TSubclassOf<UDamageNumberWidget> WidgetClass, const FTransform& Transform, int32 DmgValue, bool bForceNewInstance)
{
	if (!World || !WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Local_DisplayDamageNumber - Invalid arguments!"));
		return nullptr;
	}

	ALocalDamageNumber* ReturnInst = nullptr;
	if (bForceNewInstance || UsableLocalDamageNumberPool.IsEmpty())
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ReturnInst = World->SpawnActor<ALocalDamageNumber>(
			ALocalDamageNumber::StaticClass(),
			Transform,
			SpawnParam
		);

		if (!ReturnInst)
		{
			UE_LOG(LogTemp, Error, TEXT("Local_DisplayDamageNumber - LocalDamageNumber actor spawn failed!"));
			return nullptr;
		}

		TR_PRINT("Local_DisplayDamageNumber -  New instance");
	}
	else
	{
		ALocalDamageNumber* DmgNumberActor = *UsableLocalDamageNumberPool.Peek();
		UsableLocalDamageNumberPool.Pop();
		if (!IsValid(DmgNumberActor))
		{
			UE_LOG(LogTemp, Error, TEXT("Local_DisplayDamageNumber - LocalDamageNumber actor that was in the usable pool is invalid!"));
			ReturnInst = Local_DisplayDamageNumber(World, WidgetClass, Transform, DmgValue, bForceNewInstance);
		}
		else
		{
			ReturnInst = DmgNumberActor;
		}

		ReturnInst->SetActorTransform(Transform);
		TR_PRINT("Local_DisplayDamageNumber -  Popped from pool");
	}

	ReturnInst->SetWidgetClassAs(WidgetClass);
	ReturnInst->ReturnPool = &UsableLocalDamageNumberPool;
	ReturnInst->StartDisplay(DmgValue);
	return ReturnInst;
}

void ATRGameState::Local_OnDoorKeysChanged()
{
	// UI 업데이트
	if (!GameStateHUD) return;
	GameStateHUD->UpdateRoomKeys();
}

void ATRGameState::OnRep_DungeonDoorKeys()
{
	Local_OnDoorKeysChanged();
}
