// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/ActItemComponent.h"
#include "Items/WieldItem.h"
#include "Characters/GameCharacter.h"
#include "Characters/FPSCharacter.h"

// Sets default values for this component's properties
UActItemComponent::UActItemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Server_TriggerIntervalDelegate = FTimerDelegate::CreateUObject(this, &UActItemComponent::Server_OnTriggerTimerPassed);
	Client_TriggerIntervalDelegate = FTimerDelegate::CreateUObject(this, &UActItemComponent::Client_OnTriggerTimerPassed);
}


// Called when the game starts
void UActItemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UActItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UActItemComponent::Host_CanTrigger(AGameCharacter* Invoker)
{
	if (!IsValid(Invoker)) return false;

	// 클라는 본인 prediction 값을 사용한다
	// 클라이언트의 격발 시뮬레이션(FX) 상 격발이 불가능한 상황인 경우에 해당됨
	if (Invoker->HasAuthority())
	{
		if (!bServer_HasTriggerIntervalPassed) return false;
	}
	else
	{
		if (!bClient_HasTriggerIntervalPassed) return false;
	}
	return true; 
	// 추가 로직 필요 시 오버라이드
}

bool UActItemComponent::Trigger(AGameCharacter* Invoker)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("UActItemComponent::Trigger - component owner is invalid, or the host is not authoritative."));
		return false;
	}
	if (!Host_CanTrigger(Invoker))
	{
		UE_LOG(LogTemp, Warning, TEXT("UActItemComponent::Trigger - Invalid request. This could happen if client is not fully in-sync with the server."));
		return false;
	}

	AFPSCharacter* PlayerPawn = Cast<AFPSCharacter>(Invoker);
	bool bTriggerResult = false;
	if (PlayerPawn) 
	{
		bTriggerResult = TriggeredByPlayer(PlayerPawn);
	}
	else 
	{
		bTriggerResult = TriggeredByAI(Invoker);
	}

	if (bTriggerResult)
	{
		// 트리거 타이머 설정
		Server_StartTriggerTimer();

		AWieldItem* WieldOwner = Cast<AWieldItem>(GetOwner());
		if (WieldOwner)
		{
			return WieldOwner->OnItemTriggerProcessed(this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UActItemComponent::Trigger - Component owner is not a child of AWieldItem! Please check."));
			return false;
		}
	}
	return true;
}

bool UActItemComponent::TriggeredByPlayer(AFPSCharacter* PlayerPawn)
{
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("TriggeredByPlayer - PlayerPawn is null!"));
		return false;
	}
	return true;
}

bool UActItemComponent::TriggeredByAI(AGameCharacter* AIPawn)
{
	if (!AIPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("TriggeredByAI - Pawn is null!"));
		return false;
	}
	return true;
}

bool UActItemComponent::Stop(AGameCharacter* Invoker)
{
	AFPSCharacter* PlayerPawn = Cast<AFPSCharacter>(Invoker);
	if (PlayerPawn) {
		return StoppedByPlayer(PlayerPawn);
	}
	else {
		return StoppedByAI(Invoker);
	}
}

bool UActItemComponent::StoppedByPlayer(AFPSCharacter* PlayerPawn)
{
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("TriggeredByPlayer - PlayerPawn is null!"));
		return false;
	}
	return true;
}

bool UActItemComponent::StoppedByAI(AGameCharacter* AIPawn)
{
	if (!AIPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("TriggeredByAI - Pawn is null!"));
		return false;
	}
	return true;
}

void UActItemComponent::Local_PlayFx(AGameCharacter* Invoker)
{
	// 필요 시 오버라이드
	return;
}

void UActItemComponent::Local_StopFx(AGameCharacter* Invoker)
{
	// 필요 시 오버라이드
	return;
}

void UActItemComponent::Client_TriggerSimulate(AGameCharacter* Invoker)
{
	if (!Host_CanTrigger(Invoker))
	{
		UE_LOG(LogTemp, Warning, TEXT("UActItemComponent::Client_TriggerSimulate - Unintended usage!"));
		return;
	}

	Client_StartTriggerTimer();
}

bool UActItemComponent::IsComponentPrimary() const
{
	return (Cast<AWieldItem>(GetOwner())->PrimaryActComponent == this);
}

void UActItemComponent::Server_StartTriggerTimer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_StartTriggerTimer - World is missing!"));
		return;
	}
	if (World->GetNetMode() == ENetMode::NM_Client || !bServer_HasTriggerIntervalPassed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_StartTriggerTimer - Unintended behaviour!"));
	}
	bServer_HasTriggerIntervalPassed = false;
	World->GetTimerManager().SetTimer(Server_TriggerIntervalTimer, Server_TriggerIntervalDelegate, TriggerInterval, false);
}

void UActItemComponent::Client_StartTriggerTimer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Client_StartTriggerTimer - World is missing!"));
		return;
	}
	if (World->GetNetMode() != ENetMode::NM_Client || !bClient_HasTriggerIntervalPassed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client_StartTriggerTimer - Unintended behaviour!"));
	}
	bClient_HasTriggerIntervalPassed = false;
	World->GetTimerManager().SetTimer(Client_TriggerIntervalTimer, Client_TriggerIntervalDelegate, TriggerInterval, false);
}

void UActItemComponent::Server_OnTriggerTimerPassed()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_OnTriggerTimerPassed - World is missing!"));
		return;
	}

	World->GetTimerManager().ClearTimer(Server_TriggerIntervalTimer);
	if (Server_ShouldLoopIntervalTimer())
	{
		World->GetTimerManager().SetTimer(Server_TriggerIntervalTimer, Server_TriggerIntervalDelegate, TriggerInterval, false);
	}
	else
	{
		bServer_HasTriggerIntervalPassed = true;
	}
}

void UActItemComponent::Client_OnTriggerTimerPassed()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Client_OnTriggerTimerPassed - World is missing!"));
		return;
	}

	World->GetTimerManager().ClearTimer(Client_TriggerIntervalTimer);
	if (Client_ShouldLoopIntervalTimer())
	{
		World->GetTimerManager().SetTimer(Client_TriggerIntervalTimer, Client_TriggerIntervalDelegate, TriggerInterval, false);
	}
	else
	{
		bClient_HasTriggerIntervalPassed = true;
	}
}

