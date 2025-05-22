// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

#include "Core/TRStructs.h"
#include "TRPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ATRPlayerState : public APlayerState
{
	GENERATED_BODY()

#pragma region /** Networking */
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(ATRPlayerState, bIsOut);
	}
#pragma endregion

#pragma region /** Persistent Data */
/* Data persistency */
public:
	virtual void CopyProperties(APlayerState* PlayerState) override;

/* Gameplay */
public:
	// 플레이어 아웃 상태 변경 시 처리
	UFUNCTION()
	void OnRep_PlayerOutUpdate();

	/* Getters, Setters */
	void SetIsOut(bool Value);
	bool GetIsOut() const { return bIsOut; }

protected:
	// 플레이어가 코어 게임플레이에서 제외되어 있는지 여부
	// 이 값은 PlayerController 하위의 PlayerState에 저장되기 때문에 오직 플레이어(인간)만 보유할 수 있음
	// 이 값은 이 컨트롤러가 현재 게임플레이에 영향을 줄 수 있는지를 나타내는 값이며,
	// 단순 캐릭터의 사망과는 의미가 다름.
	// 관전 모드로 이동하는 것처럼 게임 내 게임플레이 상태가 변하는게 아닌 이상
	// 이 값에 대해 게임플레이 단에서 직접 접근하는 행위는 불필요함.
	// Level Persistent
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_PlayerOutUpdate, Category = "PlayerState")
	bool bIsOut = false;

/* Server */
public:
	// 기존 캐시된 값들을 리셋하고 주어진 플레어어들의 인스턴스 정보들을 캐싱한다
	void Server_CachePlayerInstanceData();

	// 캐시된 정보를 반환하고 무효화한다
	TPair<TSubclassOf<class AFPSCharacter>, FGameCharacterInstanceData*> Server_GetCachedPlayerInstanceData();

	// 캐시된 클래스만 반환하고, 캐시를 무효화하지는 않는다
	TSubclassOf<class AFPSCharacter> Server_SafeGetCachedPlayerInstClass() const { return Server_CachedPlayerInstClass; }

private:
	// 캐릭터 인스턴스를 복구해야 하는 상황들(사망 후 리스폰, 혹은 레벨 트랜지션)에 사용한다
	// 플레이어 사망 시에는 사망 직후의 상태를 저장하고, 레벨 트랜지션 발생 시에는 트랜지션 직전 상태를 저장한다
	// 단, 만약 사망 상태에서(즉 이미 값이 캐시된 상태에서) 트랜지션 했을 경우에는 캐시를 그대로 유지한 채 트랜지션 시키고, 
	// 별도로 복구를 바로 처리하진 않는다 (트랜지션 후에도 아직 사망한 상태이기 때문)
	// 트랜지션 후의 캐릭터 복구 과정은 다음과 같다
	// 1) 클래스 복구: 게임모드의 GetDefaultPawnClassForController_Implementation에서 처리 (관전 폰 클래스 복구는 불필요함)
	// 2) 데이터 복구: 게임모드의 PostAllHostWorldLoaded에서 처리

	// 캐시 데이터들
	// 사용하기 전에 반드시 유효성 검사를 해야 한다
	TSubclassOf<class AFPSCharacter> Server_CachedPlayerInstClass = nullptr; // 캐시 시점의 클래스타입 저장
	FGameCharacterInstanceData Server_CachedPlayerInstanceData; // 캐시 시점의 데이터 저장

	// 캐시된 정보가 유효한지 여부
	// 한 덩어리로 묶어 관리되므로, 전부 유효하거나 전부 유효하지 않다
	bool bServer_IsCachedDataValid = false;
#pragma endregion
};
