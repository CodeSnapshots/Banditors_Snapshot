// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "CoreMinimal.h"
#include "BotBTTask_MoveToMeleeTarget.generated.h"

/**
 * 밀리 공격을 위한 커스텀 이동 Task
 */
UCLASS()
class PROJECTTR_API UBotBTTask_MoveToMeleeTarget : public UBTTask_MoveTo
{
	GENERATED_BODY()
	
	UBotBTTask_MoveToMeleeTarget();

protected:
	// 실행부
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
