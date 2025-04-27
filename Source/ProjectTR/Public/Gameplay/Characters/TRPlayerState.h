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

	// 캐시된 정보로부터 플레이어 인스턴스 데이터를 찾아 반환한다
	FGameCharacterInstanceData* Server_GetCachedPlayerInstanceData();

private:
	// 캐시 데이터
	// 사용하기 전에 반드시 유효성 검사를 해야 한다
	FGameCharacterInstanceData Server_CachedPlayerInstanceData;

	// 캐시된 정보가 유효한지 여부 (Nullcheck)
	bool bServer_IsCachedPlayerInstanceDataValid = false;
#pragma endregion
};
