// Copyright (C) 2024-2025 by Haguk Kim


#include "AI/BTTasks/BotBTTask_StartAggro.h"
#include "AI/BaseAIController.h"

UBotBTTask_StartAggro::UBotBTTask_StartAggro()
{
	NodeName = TEXT("Start Aggro");
	bCreateNodeInstance = false;
}

EBTNodeResult::Type UBotBTTask_StartAggro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if (AIController)
	{
		AIController->OnAggroStart();
	}
	return EBTNodeResult::Succeeded;
}

FString UBotBTTask_StartAggro::GetStaticDescription() const
{
	return FString();
}
