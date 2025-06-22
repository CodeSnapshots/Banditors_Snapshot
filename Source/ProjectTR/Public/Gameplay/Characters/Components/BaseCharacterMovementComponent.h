// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterMovementComponent.generated.h"


UENUM(BlueprintType)
enum class EHumanoidLandState : uint8
{
	NORMAL UMETA(DisplayName = "Normal"),
	SOFT UMETA(DisplayName = "Soft"),
	HEAVY UMETA(DisplayName = "Heavy"),
};

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Slide			UMETA(DisplayName = "Slide"),
	CMOVE_MAX			UMETA(Hidden),
};

UCLASS()
class PROJECTTR_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_TR : public FSavedMove_Character
	{
	public:
		enum CompressedFlags
		{
			FLAG_Sprint = 0x10,
			FLAG_Custom_1 = 0x20,
			FLAG_Custom_2 = 0x40,
			FLAG_Custom_3 = 0x80,
		};

		uint8 Saved_bWantsToSprint : 1;

		FSavedMove_TR();

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_TR : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_TR(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

protected:
	// 리플레이 캐시
	bool bWantsToSprint = false;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual bool IsMovingOnGround() const override;
	// MOD2

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

public:
	// 네트워크 동기화 로직 인풋의 처리 진입점
	void OnInput_SprintStart();
	void OnInput_SprintStop();
	void OnInput_CrouchStart();
	void OnInput_CrouchStop();
	
public:
	UBaseCharacterMovementComponent();
	virtual void BeginPlay() override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	// 캐릭터를 반환한다
	class AGameCharacter* GetTROwner();

#pragma region /** Networking */
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(UBaseCharacterMovementComponent, DeltaMaxWalkSpeedCached);

		// TODO: 이하 값들은 봇의 경우 굳이 레플리케이션 하지 않아도 됨
		DOREPLIFETIME(UBaseCharacterMovementComponent, DeltaJumpCountCached);
		DOREPLIFETIME(UBaseCharacterMovementComponent, DeltaJumpSpeedCached);
		DOREPLIFETIME(UBaseCharacterMovementComponent, DeltaRollSpeedCached);
		DOREPLIFETIME(UBaseCharacterMovementComponent, DeltaRollDelayCached);
	}
#pragma endregion

#pragma region /** Input Cache */
// 마지막 입력을 캐싱한다
protected:
	// 이동 방향
	UPROPERTY(BlueprintReadOnly, Category = "Moving")
	float MovingForwardCached = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Moving")
	float MovingRightCached = 0.0f;

public:
	float GetMovingForward() const { return MovingForwardCached; }
	void SetMovingForward(float Value) { MovingForwardCached = Value; }

	float GetMovingRight() const { return MovingRightCached; }
	void SetMovingRight(float Value) { MovingRightCached = Value; }
#pragma endregion

#pragma region /** Defaults */
// 게임플레이 시작 시 초기화되는 값들로, BeginPlay 시점의 값들로 업데이트된다
// 이 값들의 존재의의는, 만약 런타임 도중 스텟을 다시 '기본값'으로 리셋해야 할 일이 생길 경우를 대비하기 위함이다
protected:
	float DefaultJumpZVelocity = 0.0f;
	float DefaultAirControl = 0.0f;

	float DefaultMaxWalkSpeed = 0.0f;
	float DefaultMinAnalogWalkSpeed = 0.0f;
	float DefaultMaxWalkSpeedCrouched = 0.0f;

	float DefaultMaxAcceleration = 0.0f;
	float DefaultBrakingDecelerationWalking = 0.0f;
	float DefaultGroundFriction = 0.0f;

	float DefaultGravityScale = 0.0f;
	float DefaultMass = 0.0f;

	float DefaultSlideMinEnterSpeed = 0.0f;
	float DefaultSlideOnEnterImpulse = 0.0f;
	float DefaultSlideGravityForce = 0.0f;
	float DefaultSlideFriction = 0.0f;
	float DefaultSlideStrafePower = 0.0f;

	float DefaultRollXYSpeed = 0.0f;
	float DefaultRollZSpeed = 0.0f;
	float DefaultRollDelay = 0.0f;
#pragma endregion

#pragma region /** Status Delta Sum */
public:
	// Setters
	void SetDeltaMaxWalkSpeedCached(float Value);
	void SetDeltaJumpCountCached(int32 Value);
	void SetDeltaJumpSpeedCached(float Value);
	void SetDeltaRollSpeedCached(float Value);
	void SetDeltaRollDelayCached(float Value);

	void ResetDeltaMaxWalkSpeedCached();
	void ResetDeltaJumpCountCached();
	void ResetDeltaJumpSpeedCached();
	void ResetDeltaRollSpeedCached();
	void ResetDeltaRollDelayCached();

private:
	UPROPERTY(Replicated)
	float DeltaMaxWalkSpeedCached = 0.0f;

	UPROPERTY(Replicated)
	int32 DeltaJumpCountCached = 0;

	UPROPERTY(Replicated)
	float DeltaJumpSpeedCached = 0.0f;

	UPROPERTY(Replicated)
	float DeltaRollSpeedCached = 0.0f;

	UPROPERTY(Replicated)
	float DeltaRollDelayCached = 0.0f;
#pragma endregion

#pragma region /** Sprint */
public:
	// 커스텀 무브먼트 관련 파라미터
	UPROPERTY(EditDefaultsOnly)
	float SprintMaxWalkSpeed = 1400.0f;

	UPROPERTY(EditDefaultsOnly)
	float WalkMaxWalkSpeed = 900.0f;
#pragma endregion

#pragma region /** Crouch / Slide */
/* Crouching / Sliding */
protected:
	// 슬라이딩 중인지 여부
	UPROPERTY(BlueprintReadOnly, Category = "Rolling")
	bool bIsSliding = false;

	UPROPERTY(EditDefaultsOnly)
	float SlideMinEnterSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	float SlideOnEnterImpulse = 350.f;

	UPROPERTY(EditDefaultsOnly)
	float SlideGravityForce = 4800.f;

	UPROPERTY(EditDefaultsOnly)
	float SlideFriction = 0.25f;

	UPROPERTY(EditDefaultsOnly)
	float SlideStrafePower = 0.55f;

public:
	UPROPERTY(EditDefaultsOnly)
	bool bCanSlide = false;

protected:
	bool CanSlideNow() const;
	void EnterSlide();
	void PhysSlide(float deltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;
	void OnSlideStateChanged(bool bNewState) const;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsSliding() const { return bIsSliding; }
	FORCEINLINE void SetIsSliding(bool Value) { bIsSliding = Value; OnSlideStateChanged(Value); }
#pragma endregion

#pragma region /** Jumping */
protected:
	// 로직
	virtual bool DoJump(bool bReplayingSimulatedInput) override;
	virtual bool CanAttemptJump() const override;
#pragma endregion

#pragma region /** Rolling */
protected:
	// 구르기 중인지 여부
	UPROPERTY(BlueprintReadOnly, Category = "Rolling")
	bool bIsRolling = false;

	// 구르기 속도
	// 거리와 속력에 영향을 준다
	UPROPERTY(EditDefaultsOnly, Category = "Rolling")
	float RollXYSpeed = 1500.0f;

	// 구르기 시 Z 속도
	// Z의 경우 캐릭터의 현재 속도를 오버라이드한다
	UPROPERTY(EditDefaultsOnly, Category = "Rolling")
	float RollZSpeed = 350.0f;

	// 구르기 쿨타임
	UPROPERTY(EditDefaultsOnly, Category = "Rolling")
	float RollDelay = 1.5f;

	// 구르기 prediction 지속 시간
	// 가능한 최대한 실제 구르기 지속시간과 일치하도록 설정하는 것이 좋다
	// 이는 구르기의 쿨타임을 의미하는 게 아니라,
	// LaunchCharacter로 순간적인 속도 변경에 의해 캐릭터가 크게 영향받는 시간을 의미한다
	UPROPERTY(EditDefaultsOnly, Category = "Rolling")
	float EstimatedRollTime = 0.8f;

private:
	// 구르기 쿨타임 타이머
	FTimerHandle RollCooldownTimer;

public:
	void Local_RequestRollRegister(float Forward, float Right);

protected:
	UFUNCTION(Server, Reliable)
	void Server_RegisterRollRPC(float Forward, float Right);

	// 빠르게 변화하는 이동에 대해서 로컬 prediction을 적용한다
	void Local_DoStartRolling(float Forward, float Right);
	void Local_DoEndRolling();

private:
	// 캐릭터 구르기 로직
	void RollTowards(const FVector& Direction);
#pragma endregion

#pragma region /** Landing */
public:
	// 착지 상태
	UPROPERTY(BlueprintReadWrite, Category = "Landing")
	EHumanoidLandState TRLandState = EHumanoidLandState::NORMAL;

protected:
	// 착지 타입 별 속도 하한선 지정
	UPROPERTY(BlueprintReadWrite, Category = "Landing")
	float HeavyLandingSpeed = 1200.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Landing")
	float NormalLandingSpeed = 900.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Landing")
	float SoftLandingSpeed = 300.0f;

public:
	// 착지 상태 변경
	void UpdateLandingState();
#pragma endregion

#pragma region /** Utils */
public:
	// 주어진 시간 동안 이 로컬 타깃에 한정하여 서버 동기화를 무시하고 로컬 로직을 처리한다
	// 서버에서 호출될 경우 아무 역할도 하지 않는다
	void PrepareClientPredictedMove(float PredictDuration);

protected:
	// 상대적 벡터의 x(전방), y(우측)가 주어졌을 경우 그 벡터가 캐릭터 전방을 기준으로 몇도 기울어진 값인지 계산한다
	double GetInputDirectionAngleFromForward(float Forward, float Right);

	// 현재 특정 커스텀 무브먼트 모드 사용중인지 여부
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

private:
	FTimerHandle ClientMovePredictionTimer;
#pragma endregion
};