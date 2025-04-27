// Copyright (C) 2024-2025 by Haguk Kim


#include "AI/BTTasks/BotBTTask_MoveToMeleeTarget.h"
#include "AIController.h"

#include "Characters/BotCharacter.h"

UBotBTTask_MoveToMeleeTarget::UBotBTTask_MoveToMeleeTarget()
{
	NodeName = TEXT("Custom Move To Node For Melee Attack");
	AcceptableRadius = 100.0f;
}

EBTNodeResult::Type UBotBTTask_MoveToMeleeTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABotCharacter* Bot = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Bot)
	{
		// 밀리 범위에만 들어와도 도착한 것으로 취급
		this->AcceptableRadius = Bot->Server_GetMeleeAtkRange();
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}