// Copyright (C) 2024-2025 by Haguk Kim


#include "AI/AIControllerPool.h"
#include "BrainComponent.h"
#include "AI/BaseAIController.h"
#include "Characters/BotCharacter.h"

UAIControllerPool::UAIControllerPool()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ABaseAIController* UAIControllerPool::CreateNewBaseAIController()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (!AIControllerClass)
		{
			UE_LOG(LogTemp, Error, TEXT("CreateNewBaseAIController - AIControllClass is null!"));
			return nullptr;
		}
		ABaseAIController* BaseController = World->SpawnActor<ABaseAIController>(AIControllerClass);
		if (BaseController)
		{
			// NOTE: AI컨트롤러 풀링 시 반드시 BrainComponent의 로직을 중지시켜야 크래시가 발생하지 않는다
			BaseController->HaltAILogic();
			return BaseController;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("CreateNewBaseAIController - World pointer is invalid!"));
	return nullptr;
}

void UAIControllerPool::BeginPlay()
{
	Super::BeginPlay();
	
}

ABaseAIController* UAIControllerPool::Animate(ABotCharacter* Bot, bool bForceNewController)
{
	if (!IsValid(Bot))
	{
		UE_LOG(LogTemp, Error, TEXT("UAIControllerPool::Animate - Bot is Invalid."));
		return nullptr;
	}

	ABaseAIController* NewController = nullptr;
	if (bForceNewController)
	{
		NewController = CreateNewBaseAIController();
	}
	else
	{
		if (Pool.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("UAIControllerPool::Animate - Controller is Invalid. Creating new one!"));
			NewController = CreateNewBaseAIController();
		}
		else
		{
			NewController = *Pool.Peek();
			Pool.Pop();
		}
	}

	if (NewController)
	{
		NewController->Possess(Bot);
		NewController->StartAILogic();
		//NewController->PrintDebug();
		return NewController;
	}
	UE_LOG(LogTemp, Error, TEXT("UAIControllerPool::Animate - Something went wrong!"));
	return nullptr;
}

void UAIControllerPool::Inanimate(ABotCharacter* Bot)
{
	if (!IsValid(Bot))
	{
		UE_LOG(LogTemp, Error, TEXT("UAIControllerPool::Inanimate - Bot is Invalid."));
		return;
	}
	ABaseAIController* AIController = Cast<ABaseAIController>(Bot->GetController());
	if (!IsValid(AIController))
	{
		UE_LOG(LogTemp, Error, TEXT("UAIControllerPool::Inanimate - Bot's controller is Invalid. IsNull: %d"), (AIController == nullptr));
		return;
	}

	AIController->HaltAILogic();
	AIController->UnPossess();

	Pool.Enqueue(AIController);
	return;
}

void UAIControllerPool::Initialize(int32 PoolSize)
{
	for (int Index = 0; Index < PoolSize; ++Index)
	{
		ABaseAIController* AIController = CreateNewBaseAIController();
		if (!AIController)
		{
			UE_LOG(LogTemp, Error, TEXT("UAIControllerPool::Fill - Bot's controller is Invalid. IsNull: %d"), (AIController == nullptr));
			return;
		}
		Pool.Enqueue(AIController);
	}
}

void UAIControllerPool::DestroyPool()
{
	DestroyComponent();
}
