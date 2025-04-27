// Copyright (C) 2025 by Haguk Kim


#include "AI/BTTasks/BotBTTask_FindRandLocToPlayer.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Core/TRMacros.h"
#include "Core/ProjectTRGameModeBase.h"
#include "Characters/FPSCharacter.h"

UBotBTTask_FindRandLocToPlayer::UBotBTTask_FindRandLocToPlayer()
{
	NodeName = TEXT("Find Random Location Towards Closest Player");
}

EBTNodeResult::Type UBotBTTask_FindRandLocToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	FNavLocation Location;

	if (!GetWorld()) return EBTNodeResult::Failed;
	AProjectTRGameModeBase* TRGM = GetWorld()->GetAuthGameMode<AProjectTRGameModeBase>();
	if (!TRGM) return EBTNodeResult::Failed;

	// 폰 획득
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
	const APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	// 플레이어 방향으로 위치한 원 내부의 랜덤한 위치 선택
	FVector PawnLocation = AIPawn->GetActorLocation();
	AFPSCharacter* Target = TRGM->FindClosestActivePlayerChar(PawnLocation);
	if (!Target)
	{
		return FindRandomLocation(OwnerComp, NodeMemory);
	}
	FVector ToPlayerVector = (Target->GetActorLocation() - PawnLocation).GetSafeNormal() * (SearchRadius * PlayerAttractionStrength);
	ToPlayerVector.Z = 0;

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (IsValid(NavSystem) && NavSystem->GetRandomPointInNavigableRadius(PawnLocation + ToPlayerVector, SearchRadius, Location))
	{
		AIController->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, Location.Location);

		TR_PRINT("ToPlayerFound!");
		DrawDebugSphere(GetWorld(), PawnLocation + ToPlayerVector, 10, 10, FColor::Yellow, false, 3.0f);
		DrawDebugLine(GetWorld(), PawnLocation + ToPlayerVector, Location.Location, FColor::Yellow, false, 3.0f);
		DrawDebugSphere(GetWorld(), Location.Location, 10, 10, FColor::White, false, 3.0f);
	}
	else
	{
		return FindRandomLocation(OwnerComp, NodeMemory);
	}

	// 태스크 종료
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBotBTTask_FindRandLocToPlayer::FindRandomLocation(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FNavLocation Location;

	// 폰 획득
	AAIController* AIController = OwnerComp.GetAIOwner();
	const APawn* AIPawn = AIController->GetPawn();

	// 자신 주변 랜덤한 위치 선택
	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (IsValid(NavSystem) && NavSystem->GetRandomPointInNavigableRadius(AIPawn->GetActorLocation(), SearchRadius, Location))
	{
		AIController->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, Location.Location);

		TR_PRINT("RandFound!");
		DrawDebugSphere(GetWorld(), Location.Location, 10, 10, FColor::Red, false, 0.5f);
	}

	// 태스크 종료
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBotBTTask_FindRandLocToPlayer::GetStaticDescription() const
{
	return FString("Choose random location within circle's radius. The center of the circle points to the closest player location at the moment.");
}
