// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/SpectatorPawn.h"
#include "InputActionValue.h"

#include "Core/TRStructs.h"
#include "TRSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ATRSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

	ATRSpectatorPawn();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual FBox GetComponentsBoundingBox(bool bNonColliding, bool bIncludeFromChildActors) const override;

#pragma region /** Networking */
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(ATRSpectatorPawn, SpectatingTarget);
	}

	// 관전 중인 타깃 변경 시
	UFUNCTION()
	void OnRep_SpecTargetChange();
#pragma endregion

#pragma region /** Input */
public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 로컬 플레이어에게 InputMappingContext를 추가한다
	void AddLocalPlayerInputMappingContext(const class UInputMappingContext* Context, int32 Priority, bool bClearAllMappings);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputMappingContext> DefaultInputMapping = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputConfig> InputConfig = nullptr;
#pragma endregion

#pragma region /** Action */
protected:
	// 이동
	void Move(const FInputActionValue& Value);

	// 시야 이동
	void Look(const FInputActionValue& Value);

	// 상하 이동
	void Float(const FInputActionValue& Value);
	void Fall(const FInputActionValue& Value);

	// 다음 플레이어로 관전 화면 전환
	void SpecChangeNext(const FInputActionValue& Value);

	// 이전 플레이어로 관전 화면 전환
	void SpecChangePrev(const FInputActionValue& Value);

protected:
	UPROPERTY(EditDefaultsOnly)
	float FloatingSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float FallSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float RotationSensitivity = 0.15f;

private:
	// 액션 처리 허용 여부
	bool bLocal_EnableMoveInput = false;
	bool bLocal_EnableLookInput = false;
#pragma endregion

public:
	// 다음 혹은 이전 타깃으로 관전 대상을 변경한다
	// Direction true일 경우 다음, false 일 경우 이전을 의미한다
	UFUNCTION(Server, Reliable)
	void Server_ChangeSpecTargetRPC(bool Direction);

	// 리스폰을 처리한다
	// 호출 시 Destroy가 호출되어 다음 틱에 이 폰은 파괴된다
	// 디버깅용으로만 사용된다
	UFUNCTION(Server, Reliable)
	void Server_RespawnPlayerForDebugRPC();

	// 타깃을 등록한다
	// 기존과 타깃이 동일할 경우 Refresh 해주는 역할을 한다
	void Server_SetSpectatingTarget(ACharacter* NewTarget);

protected:
	// 타깃이 기존과 달라졌을 경우 호출하는 함수
	// 호스트에 따른 로직 분기를 처리
	void Local_OnSpecTargetChange();

	// 새 대상에게 뷰타겟이 설정되었을 때 필요 시 추가 작업을 수행한다
	// 공통 로직
	void Local_OnViewTargetSet();

	// 주어진 타깃이 관전 가능한지 여부
	bool Server_IsTargetSpectatable(TWeakObjectPtr<ACharacter> Target);

public:
	/* Getters */
	class ACharacter* Local_GetSpectatingTarget() { return SpectatingTarget; }

protected:
	// NOTE: 리스폰 시 사용할 정보는 PlayerState에 기록

	// 현재 관전중인 대상
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpecTargetChange, Category = "Spectation")
	class ACharacter* SpectatingTarget = nullptr;

	// 바로 이전 관전했던 대상
	TWeakObjectPtr<class ACharacter> PrevSpectatingTarget = nullptr;
};
