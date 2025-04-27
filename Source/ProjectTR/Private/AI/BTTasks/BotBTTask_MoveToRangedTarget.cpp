// Copyright (C) 2024-2025 by Haguk Kim


#include "AI/BTTasks/BotBTTask_MoveToRangedTarget.h"
#include "AIController.h"
#include "Characters/BotCharacter.h"

UBotBTTask_MoveToRangedTarget::UBotBTTask_MoveToRangedTarget()
{
	NodeName = TEXT("Custom Move To Node For Ranged Attack");
	AcceptableRadius = 500.0f;
}

EBTNodeResult::Type UBotBTTask_MoveToRangedTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABotCharacter* Bot = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Bot)
	{
		this->AcceptableRadius = Bot->Server_GetRangedAtkRange();
		this->ObservedBlackboardValueTolerance = this->AcceptableRadius * 0.95;
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
