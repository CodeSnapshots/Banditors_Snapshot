// Copyright (C) 2025 by Haguk Kim


#include "AI/BTTasks/BotBTTask_WaitMovement.h"
#include "AI/BaseAIController.h"

UBotBTTask_WaitMovement::UBotBTTask_WaitMovement()
{
	NodeName = TEXT("WaitMovement");
}

EBTNodeResult::Type UBotBTTask_WaitMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if (AIController)
	{
		WaitTime = AIController->GetConsecutiveMovementFailure() * WaitPerFailCount;
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

FString UBotBTTask_WaitMovement::GetStaticDescription() const
{
	return FString("Waits proportional to consecutive movement fail count");
}
