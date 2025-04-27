// Copyright (C) 2024-2025 by Haguk Kim


#include "AI/BTTasks/BotBTTask_Unexpected.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Core/TRMacros.h"
#include "AI/BaseAIController.h"

UBotBTTask_Unexpected::UBotBTTask_Unexpected()
{
	NodeName = TEXT("Called upon unexpected AI behaviour");
}

EBTNodeResult::Type UBotBTTask_Unexpected::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController))
	{
		UBlackboardComponent* BB = AIController->GetBlackboardComponent();
		if (!BB)
		{
			UE_LOG(LogTemp, Error, TEXT("UBotBTTask_Unexpected - Blackboard is nullptr!"));
		}
		UE_LOG(LogTemp, Warning, TEXT("UBotBTTask_Unexpected - Something went wrong!"));
	}

	// 태스크 종료
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
