// Copyright (C) 2024-2025 by Haguk Kim


#include "AI/BTTasks/BotBTTask_TargetUnreachable.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Core/TRMacros.h"
#include "AI/BaseAIController.h"

UBotBTTask_TargetUnreachable::UBotBTTask_TargetUnreachable()
{
	NodeName = TEXT("Target is unreachable");
}

EBTNodeResult::Type UBotBTTask_TargetUnreachable::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController))
	{
		AIController->ClearTarget();
	}

	// 태스크 종료
	return EBTNodeResult::Failed;
}

FString UBotBTTask_TargetUnreachable::GetStaticDescription() const
{
	return FString("LEGACY: use TargetUpdate instead");
}
