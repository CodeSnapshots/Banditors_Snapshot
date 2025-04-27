// Copyright (C) 2024-2025 by Haguk Kim


#include "AI/BTTasks/BotBTTask_AimAtTarget.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Core/TRMacros.h"
#include "AI/BaseAIController.h"
#include "Characters/BotCharacter.h"

UBotBTTask_AimAtTarget::UBotBTTask_AimAtTarget()
{
	NodeName = TEXT("Aim towards target");
	bCreateNodeInstance = true; // 액터별로 프로퍼티 값이 달라질 수 있음
	bNotifyTick = true;
	Precision = 10.0f;
}

EBTNodeResult::Type UBotBTTask_AimAtTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type SuperResult = Super::ExecuteTask(OwnerComp, NodeMemory);
	if (SuperResult != EBTNodeResult::Succeeded)
	{
		return SuperResult;
	}

	ABotCharacter* Bot = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Bot)
	{
		return EBTNodeResult::Failed;
	}

	if (!bInitialized)
	{
		bInitialized = true;
		if (Bot)
		{
			// 각도 허용치 조정
			Precision = Bot->Server_GetRangedAtkRotPrecision();
			YawRotateSpeed = Bot->Server_GetRangedAtkRotSpeed();
		}
	}
	return EBTNodeResult::InProgress;
}

void UBotBTTask_AimAtTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ABotCharacter* Bot = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if (!Bot || !AIController)
	{
		return FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	TWeakObjectPtr<AActor> WeakPtrCopy = AIController->GetCurrentTarget();
	if (!WeakPtrCopy.IsValid())
	{
		return FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	AActor* TargetActor = WeakPtrCopy.Get(); // 원본 WeakPtr를 Deref하지 않음

	FVector CurrentLocation = Bot->GetActorLocation();
	FRotator CurrentRotation = Bot->GetActorRotation();
	FVector DirectionToTarget = (TargetActor->GetActorLocation() - CurrentLocation).GetSafeNormal();
	FRotator TargetRotation = DirectionToTarget.Rotation();

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, YawRotateSpeed);
	NewRotation.Pitch = 0;
	NewRotation.Roll = 0;
	Bot->SetActorRotation(NewRotation);

	if (FMath::Abs(FRotator::NormalizeAxis(TargetRotation.Yaw - NewRotation.Yaw)) <= Precision)
	{
		if (bShouldCheckLOS)
		{
			if (Bot->IsAimingAtTarget(TargetActor))
			{
				return FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			return FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
		return FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}


FString UBotBTTask_AimAtTarget::GetStaticDescription() const
{
	return FString("Rotates AI towards target. if LOS should be checked and something is blocking LOS to target return fail.");
}


