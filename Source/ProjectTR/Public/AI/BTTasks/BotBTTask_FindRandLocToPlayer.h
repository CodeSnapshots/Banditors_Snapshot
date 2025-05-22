// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BotBTTask_FindRandLocToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API UBotBTTask_FindRandLocToPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBotBTTask_FindRandLocToPlayer();

private:
	// 실행부
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 에러 발생 등으로 인해 플레이어를 특정하지 못하는 경우, 자신 주변의 랜덤한 위치를 찾는 로직을 대신 실행한다
	EBTNodeResult::Type FindRandomLocation(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	// BP node description
	virtual FString GetStaticDescription() const override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = true))
	float SearchRadius = 1500.0f;

	// PlayerAttractionStrength는 최초 서치 원의 중심이 플레이어에게 얼마나 가까운지 결정한다
	// 값이 1일경우 AI오너에서 SearchRadius만큼 떨어진 지점에 원의 중심이 위치하게 된다
	// 지나친 플레이어 방향 한쪽으로의 편향을 막기 위해 값은 1 미만으로 하는 게 권장된다
	// 이 값은 Intensity 값에 의해 Lerping 된다 (DDA)
	// Intensity와 무관하게 고정값을 주고 싶다면 Min = Max로 두면 된다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = true))
	float MinPlayerAttractionStrength = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = true))
	float MaxPlayerAttractionStrength = 1.0f;
};
