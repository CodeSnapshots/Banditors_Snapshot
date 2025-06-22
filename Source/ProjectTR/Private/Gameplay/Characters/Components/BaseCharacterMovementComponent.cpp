// Copyright (C) 2024-2025 by Haguk Kim


#include "Characters/Components/BaseCharacterMovementComponent.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

#include "Characters/GameCharacter.h"
#include "DataAssets/AnimConfig.h"

UBaseCharacterMovementComponent::UBaseCharacterMovementComponent()
{
	// Replication 사용
	SetIsReplicatedByDefault(true);

	/* 기본값 */
	bUseControllerDesiredRotation = true; // 컨트롤러 회전 사용; FPS 카메라가 이동 시 돌아가는 걸 방지
	bOrientRotationToMovement = false; // 인풋 방향 지향
	NavAgentProps.bCanCrouch = true;
	bCanWalkOffLedgesWhenCrouching = true;

	bComponentShouldUpdatePhysicsVolume = false;
	
	RotationRate = FRotator(0.0f, 240.0f, 0.0f); // 회전 속도

	// UNavMovementComponent 하위 프로퍼티
	// 커스텀 캐릭터 무브먼트 컴포넌트를 정의해야만 C++에서 디폴트값 재정의 가능
	SetFixedBrakingDistance(200.0f); // 기본값: NavMesh 상에서 목적지에 다다르기 200cm 전 감속을 하도록 설정, 상황에 맞게 수정할 것
	bUseAccelerationForPaths = true;
	bUseFixedBrakingDistanceForPaths = true;

	GravityScale = 2.35;
	MaxAcceleration = 12000.0f;
	BrakingFrictionFactor = 1.85f;
	GroundFriction = 4.0f;
	bMaintainHorizontalGroundVelocity = false;
	JumpZVelocity = 775.0f;
}

void UBaseCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// 인스턴스 런타임 기본값 설정;
	// 에디터에서 지정된 값들로 설정된다
	DefaultJumpZVelocity = JumpZVelocity;
	DefaultAirControl = AirControl;

	DefaultMaxWalkSpeed = MaxWalkSpeed;
	DefaultMinAnalogWalkSpeed = MinAnalogWalkSpeed;
	DefaultMaxWalkSpeedCrouched = MaxWalkSpeedCrouched;

	DefaultBrakingDecelerationWalking = BrakingDecelerationWalking;
	DefaultMaxAcceleration = MaxAcceleration;
	DefaultGroundFriction = GroundFriction;

	DefaultGravityScale = GravityScale;
	DefaultMass = Mass;

	DefaultSlideMinEnterSpeed = SlideMinEnterSpeed;
	DefaultSlideOnEnterImpulse = SlideOnEnterImpulse;
	DefaultSlideGravityForce = SlideGravityForce;
	DefaultSlideFriction = SlideFriction;
	DefaultSlideStrafePower = SlideStrafePower;

	DefaultRollXYSpeed = RollXYSpeed;
	DefaultRollZSpeed = RollZSpeed;
	DefaultRollDelay = RollDelay;
}

void UBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	bool bWasModeSliding = (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Slide);
	bool bIsModeSliding = (MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_Slide);
	if (!bWasModeSliding && bIsModeSliding)
	{
		SetIsSliding(true);
	}
	if (bWasModeSliding && !bIsModeSliding)
	{
		SetIsSliding(false);
	}
}

AGameCharacter* UBaseCharacterMovementComponent::GetTROwner()
{
	return Cast<AGameCharacter>(GetCharacterOwner());
}

void UBaseCharacterMovementComponent::Server_RegisterRollRPC_Implementation(float Forward, float Right)
{
	if (GetOwnerRole() < ROLE_Authority) return;
	Local_DoStartRolling(Forward, Right);
}

void UBaseCharacterMovementComponent::SetDeltaMaxWalkSpeedCached(float Value)
{
	DeltaMaxWalkSpeedCached = Value;
	// NOTE: 추가 로직 필요 없음
}

void UBaseCharacterMovementComponent::SetDeltaJumpCountCached(int32 Value)
{
	ACharacter* Owner = GetCharacterOwner();
	if (!Owner) return;
	DeltaJumpCountCached = Value;
	Owner->JumpMaxCount = FMath::Max(1, 1 + DeltaJumpCountCached);
}

void UBaseCharacterMovementComponent::SetDeltaJumpSpeedCached(float Value)
{
	DeltaJumpSpeedCached = Value;
	JumpZVelocity = DefaultJumpZVelocity + DeltaJumpSpeedCached;
}

void UBaseCharacterMovementComponent::SetDeltaRollSpeedCached(float Value)
{
	DeltaRollSpeedCached = Value;
	RollXYSpeed = FMath::Max(0.0f, DefaultRollXYSpeed + DeltaRollSpeedCached);
}

void UBaseCharacterMovementComponent::SetDeltaRollDelayCached(float Value)
{
	DeltaRollDelayCached = Value;
	RollDelay = FMath::Max(0.0f, DefaultRollDelay + DeltaRollDelayCached);
}

void UBaseCharacterMovementComponent::ResetDeltaMaxWalkSpeedCached()
{
	SetDeltaMaxWalkSpeedCached(0.0f);
}

void UBaseCharacterMovementComponent::ResetDeltaJumpCountCached()
{
	SetDeltaJumpCountCached(0);
}

void UBaseCharacterMovementComponent::ResetDeltaJumpSpeedCached()
{
	SetDeltaJumpSpeedCached(0.0f);
}

void UBaseCharacterMovementComponent::ResetDeltaRollSpeedCached()
{
	SetDeltaRollSpeedCached(0.0f);
}

void UBaseCharacterMovementComponent::ResetDeltaRollDelayCached()
{
	SetDeltaRollDelayCached(0.0f);
}

bool UBaseCharacterMovementComponent::CanSlideNow() const
{
	if (bCanSlide && IsCrouching())
	{
		FHitResult PotentialSlideSurface;
		if (Velocity.SizeSquared() > pow(SlideMinEnterSpeed, 2) && GetSlideSurface(PotentialSlideSurface))
		{
			return true;
		}
	}
	return false;
}

void UBaseCharacterMovementComponent::EnterSlide()
{
	Velocity += Velocity.GetSafeNormal2D() * SlideOnEnterImpulse;
	SetMovementMode(MOVE_Custom, CMOVE_Slide);
}

void UBaseCharacterMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CanSlideNow())
	{
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;

		// 경사면 가속
		FVector SlopeForce = CurrentFloor.HitResult.Normal;
		SlopeForce.Z = 0.f;
		Velocity += SlopeForce * SlideGravityForce * deltaTime;

		// 스트레이핑 처리
		const double VelocitySizeBeforeStrafe = Velocity.Size();

		// 전방 기준 좌우 30도 내의 가속도(=앞으로 가려는 영향력)는 반영하지 않는다
		if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetForwardVector())) < 0.87f)
		{
			Velocity += Acceleration * SlideStrafePower * deltaTime;

			// 스트레이핑을 통해서 속도가 증가하는 일이 발생해서는 안된다
			Velocity = Velocity.GetClampedToMaxSize(VelocitySizeBeforeStrafe);
		}

		// 애니메이션에서 Acceleration 값을 이용하기 때문에 영벡터로 만들어준다
		// 부자연스러운 다리 움직임을 막기 위함
		Acceleration = FVector::ZeroVector;

		// 추후 땅 종류별로 마찰을 변경해야 할 경우 여기서 SlideFriction 대신 GroundFriction * SlideFrictionFactor로 변경 가능
		CalcVelocity(timeTick, SlideFriction, false, GetMaxBrakingDeceleration());

		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;
		bool bFloorWalkable = CurrentFloor.IsWalkableFloor();

		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);
			if (IsFalling())
			{
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
			else if (IsSwimming())
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}

		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
			if (!NewDelta.IsZero())
			{
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);
				bTriedLedgeMove = true;
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;
			}
		}

		if (IsMovingOnGround() && bFloorWalkable)
		{
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, UpdatedComponent->GetComponentRotation(), false, Hit);
}

bool UBaseCharacterMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	ACharacter* CharOwner = GetCharacterOwner();
	if (!CharOwner || !GetWorld()) return false;
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 3.0f * FVector::DownVector;
	FCollisionQueryParams ColParams;
	ColParams.AddIgnoredActor(CharOwner);
	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, TEXT("PlayerCapsule"), ColParams);
}

void UBaseCharacterMovementComponent::OnSlideStateChanged(bool bNewState) const
{
	AGameCharacter* CharOwner = Cast<AGameCharacter>(GetCharacterOwner());
	if (CharOwner)
	{
		if (CharOwner->GetLocalRole() == ROLE_Authority)
		{
			CharOwner->Multicast_SetSlideFxRPC(bNewState);
		}
		if (CharOwner->IsLocallyControlled())
		{
			CharOwner->Local_SetSlideFx(bNewState);
		}
	}
}

bool UBaseCharacterMovementComponent::DoJump(bool bReplayingSimulatedInput)
{
	return Super::DoJump(bReplayingSimulatedInput);
}

bool UBaseCharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed() &&
		(!bWantsToCrouch || GetIsSliding()/* 슬라이딩 도중 점프 가능 */) &&
		(IsMovingOnGround() || IsFalling());
}

void UBaseCharacterMovementComponent::Local_RequestRollRegister(float Forward, float Right)
{
	if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
	{
		PrepareClientPredictedMove(EstimatedRollTime);
		Local_DoStartRolling(Forward, Right);
		Server_RegisterRollRPC(Forward, Right);
	}
	else if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		Server_RegisterRollRPC(Forward, Right);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Local_RequestRollRegister - Simulated proxies should not call this function!"));
	}
}

void UBaseCharacterMovementComponent::Local_DoStartRolling(float Forward, float Right)
{
	// NOTE: 추후 필요 시 IsFalling() 체크 가능
	if (IsCrouching() || bIsRolling)
	{
		return;
	}

	AGameCharacter* GameCharacter = Cast<AGameCharacter>(GetCharacterOwner());
	if (GameCharacter)
	{
		// 머즐 XY 방향이 전방
		FVector ForwardDirection = GameCharacter->GetMuzzleInfo().Get<1>().Vector();
		ForwardDirection.Z = 0;
		ForwardDirection.Normalize();

		FVector2D ForwardDirection2D = FVector2D(ForwardDirection.X, ForwardDirection.Y);
		ForwardDirection2D = ForwardDirection2D.GetRotated(GetInputDirectionAngleFromForward(Forward, Right));

		RollTowards(FVector(ForwardDirection2D.X, ForwardDirection2D.Y, 0));
	}

	// 쿨다운 적용
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(RollCooldownTimer, this, &UBaseCharacterMovementComponent::Local_DoEndRolling, RollDelay, false);
	}
}

void UBaseCharacterMovementComponent::Local_DoEndRolling()
{
	UWorld* World = GetWorld();
	if (World && World->GetTimerManager().IsTimerActive(RollCooldownTimer))
	{
		World->GetTimerManager().ClearTimer(RollCooldownTimer);
	}
}

double UBaseCharacterMovementComponent::GetInputDirectionAngleFromForward(float Forward, float Right)
{
	if (FMath::IsNearlyZero(Forward) && FMath::IsNearlyZero(Right))
	{
		return 0.0f;
	}

	FVector InputDirection = FVector(Forward, Right, 0.0f).GetSafeNormal();
	float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::ForwardVector, InputDirection)));
	FVector CrossProduct = FVector::CrossProduct(FVector::ForwardVector, InputDirection);
	if (CrossProduct.Z < 0)
	{
		AngleDegrees = -AngleDegrees;
	}
	return AngleDegrees;
}

bool UBaseCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UBaseCharacterMovementComponent::PrepareClientPredictedMove(float PredictDuration)
{
	if (!GetWorld() || GetOwnerRole() != ENetRole::ROLE_AutonomousProxy)
	{
		return;
	}

	bIgnoreClientMovementErrorChecksAndCorrection = true;
	GetWorld()->GetTimerManager().ClearTimer(ClientMovePredictionTimer);
	GetWorld()->GetTimerManager().SetTimer(
		ClientMovePredictionTimer,
		[this]() {
			bIgnoreClientMovementErrorChecksAndCorrection = false;
		}, 
		PredictDuration, 
		false
	);
}

void UBaseCharacterMovementComponent::RollTowards(const FVector& Direction)
{
	if (!GetCharacterOwner()) return;
	if (!Direction.IsNearlyZero())
	{
		float OriginSpeed = GetLastUpdateVelocity().Size();
		FVector NewVelocity = GetLastUpdateVelocity() + (Direction.GetSafeNormal() * RollXYSpeed);
		NewVelocity.Z = 0;

		// 이미 속도가 롤링 속도보다 빠를 경우 더 빨라지지 않는다
		NewVelocity = NewVelocity.GetSafeNormal() * FMath::Max(OriginSpeed, RollXYSpeed);

		// 지면에 있을 경우에만 Z속도를 오버라이드해 띄워올린다
		if (!IsFalling())
		{
			NewVelocity.Z = RollZSpeed;
		}
		// XY 속도는 항상 오버라이드한다
		GetCharacterOwner()->LaunchCharacter(NewVelocity, true, !IsFalling());
	}
}

void UBaseCharacterMovementComponent::UpdateLandingState()
{
	float ZSpeed = FMath::Abs(Velocity.Z);
	if (ZSpeed > HeavyLandingSpeed)
	{
		TRLandState = EHumanoidLandState::HEAVY;
	}
	else if (ZSpeed > NormalLandingSpeed)
	{
		TRLandState = EHumanoidLandState::NORMAL;
	}
	else if (ZSpeed > SoftLandingSpeed)
	{
		TRLandState = EHumanoidLandState::SOFT;
	}
	else
	{
		// NONE
	}
}

UBaseCharacterMovementComponent::FSavedMove_TR::FSavedMove_TR()
{
	Saved_bWantsToSprint = 0;
}

bool UBaseCharacterMovementComponent::FSavedMove_TR::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_TR* NewTRMove = static_cast<FSavedMove_TR*>(NewMove.Get());
	if (Saved_bWantsToSprint != NewTRMove->Saved_bWantsToSprint)
	{
		return false;
	}
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UBaseCharacterMovementComponent::FSavedMove_TR::Clear()
{
	FSavedMove_Character::Clear();
	Saved_bWantsToSprint = 0;
}

uint8 UBaseCharacterMovementComponent::FSavedMove_TR::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();
	if (Saved_bWantsToSprint) Result |= FLAG_Sprint;
	return Result;
}

void UBaseCharacterMovementComponent::FSavedMove_TR::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	const UBaseCharacterMovementComponent* CharacterMovement = Cast<UBaseCharacterMovementComponent>(C->GetCharacterMovement());
	Saved_bWantsToSprint = CharacterMovement->bWantsToSprint;
}

void UBaseCharacterMovementComponent::FSavedMove_TR::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);
	UBaseCharacterMovementComponent* CharacterMovement = Cast<UBaseCharacterMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->bWantsToSprint = Saved_bWantsToSprint;
}

UBaseCharacterMovementComponent::FNetworkPredictionData_Client_TR::FNetworkPredictionData_Client_TR(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UBaseCharacterMovementComponent::FNetworkPredictionData_Client_TR::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_TR());
}

FNetworkPredictionData_Client* UBaseCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)

	if (ClientPredictionData == nullptr)
	{
		UBaseCharacterMovementComponent* MutableThis = const_cast<UBaseCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_TR(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	return ClientPredictionData;
}

bool UBaseCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

void UBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToSprint = (Flags & FSavedMove_TR::FLAG_Sprint) != 0;
}

void UBaseCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// 크라우칭을 여기서 먼저 처리
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	bool bCanSlideNow = CanSlideNow();
	if (MovementMode == MOVE_Walking && bCanSlideNow)
	{
		EnterSlide();
	}
}

void UBaseCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (bWantsToSprint)
		{
			MaxWalkSpeed = SprintMaxWalkSpeed + DeltaMaxWalkSpeedCached;
		}
		else
		{
			MaxWalkSpeed = WalkMaxWalkSpeed + DeltaMaxWalkSpeedCached;
		}
	}
}

void UBaseCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

void UBaseCharacterMovementComponent::OnInput_SprintStart()
{
	AGameCharacter* TROwner = GetTROwner();
	if (TROwner && !TROwner->bIsCrouched)
	{
		bWantsToSprint = true;
	}
}

void UBaseCharacterMovementComponent::OnInput_SprintStop()
{
	bWantsToSprint = false;
}

void UBaseCharacterMovementComponent::OnInput_CrouchStart()
{
	bWantsToCrouch = true;
}

void UBaseCharacterMovementComponent::OnInput_CrouchStop()
{
	bWantsToCrouch = false;
}
