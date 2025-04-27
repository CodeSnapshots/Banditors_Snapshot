// Copyright (C) 2024-2025 by Haguk Kim


#include "AI/BTTasks/BotBTTask_OnAttackerLastLocVisit.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Core/TRMacros.h"
#include "AI/BaseAIController.h"

UBotBTTask_OnAttackerLastLocVisit::UBotBTTask_OnAttackerLastLocVisit()
{
	NodeName = TEXT("Attacker last known location visited or it is unreachable");
}

EBTNodeResult::Type UBotBTTask_OnAttackerLastLocVisit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController))
	{
		AIController->ClearAttackerLastLocation();
	}

	// 태스크 종료
	return EBTNodeResult::Succeeded;
}

FString UBotBTTask_OnAttackerLastLocVisit::GetStaticDescription() const
{
	return FString("Resets attacker last known location and reference");
}

