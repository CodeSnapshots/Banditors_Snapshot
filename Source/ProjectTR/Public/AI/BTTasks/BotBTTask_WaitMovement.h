// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BotBTTask_WaitMovement.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UBotBTTask_WaitMovement : public UBTTask_Wait
{
	GENERATED_BODY()
	
public:
	UBotBTTask_WaitMovement();

	// 실행부
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;	
	
	// BP node description
	virtual FString GetStaticDescription() const override;

public:
	// 매 실패 횟수마다 더해지는 값 (linear 증가)
	// 기본 WaitTime을 무시하고 이 값이 사용된다
	UPROPERTY(Category = Wait, EditAnywhere, meta = (UIMin = 0, ClampMin = 0))
	float WaitPerFailCount = 2.0f;
};
