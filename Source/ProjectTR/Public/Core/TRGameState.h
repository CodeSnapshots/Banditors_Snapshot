// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "Math/Color.h"

#include "UI/LocalDamageNumber.h"
#include "UI/TRGameStateHUD.h"
#include "TRGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ATRGameState : public AGameStateBase // Mixing AGameState with AGameModeBase is not compatible
{
	GENERATED_BODY()

#pragma region /** Networking */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(ATRGameState, DungeonTimeLeft);
		DOREPLIFETIME(ATRGameState, DungeonDoorKeys);
		DOREPLIFETIME(ATRGameState, BossCharacters);
		DOREPLIFETIME(ATRGameState, bGameOver);
	}
#pragma endregion

public:
	void BeginPlay() override;

#pragma region /** Gameplay */
protected:
	// 현재 던전 입장 후 경과 시간 (초)
	UPROPERTY(ReplicatedUsing = OnRep_DungeonTimeLeft, BlueprintReadOnly)
	int32 DungeonTimeLeft;

	// 게임 오버 여부
	// NOTE: 서버의 경우 GameState에서 이 정보에 접근하기보다는 가급적 GameMode를 사용할 것
	UPROPERTY(ReplicatedUsing = OnRep_GameOver, BlueprintReadOnly)
	bool bGameOver = false;

private:
	// 현재 던전에서 획득한 보유 키들의 목록
	// 각 키들은 레벨 단위로만 관리된다
	UPROPERTY(ReplicatedUsing = OnRep_DungeonDoorKeys)
	TArray<int32> DungeonDoorKeys;

	UPROPERTY(ReplicatedUsing = OnRep_BossCharacters)
	TArray<class AGameCharacter*> BossCharacters;

public:
	// 던전 시간 Getters, Setters
	int32 Local_GetDungeonTimeLeft() { return DungeonTimeLeft; }
	void Server_SetDungeonTimeLeft(int32 Value);

	// 스트링 형태로 변환해 반환
	UFUNCTION(BlueprintCallable)
	FString GetDungeonTimeString();

	// 레드모드 진입 시 각 호스트에서 처리해주어야 하는 로직을 처리한다
	void Server_ProcessRedModeEnter();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ProcessRedModeEnter();

	// 레벨 내의 모든 해당 타입 라이트들의 밝기 및 색상을 재설정한다
	// 색상 포인터가 nullptr일 경우 변경하지 않는다
	template<typename T>
	void Local_SetAllLightSettings(float BrightnessMult, FString TagName, FLinearColor* Color)
	{
		static_assert(TIsDerivedFrom<T, ALight>::Value, "Local_SetAllLightSettings - T must be derived from ALight!");

		UWorld* World = GetWorld();
		if (!World)
		{
			return;
		}
		for (TActorIterator<T> It(World); It; ++It)
		{
			T* Light = *It;
			if (Light)
			{
				// 액터 인스턴스 태그를 추가해주어야 한다
				if (Light->ActorHasTag(FName(*TagName)))
				{
					Light->SetBrightness(Light->GetBrightness() * BrightnessMult);
					if (Color) Light->SetLightColor(*Color);
				}
			}
		}
	}

	// 던전 키 획득 / 소비
	// 주어진 Id의 키를 추가한다
	void Server_AddDoorKey(int32 KeyId);

	// 주어진 Id의 키를 소비하고, 소비 여부를 반환한다
	bool Server_UseDoorKey(int32 KeyId);

	// 주어진 Id 이하의 값을 가지는 Id의 키를 소비하고, 소비 여부를 반환한다
	// 하나의 키로 여러 문을 다 열 수 있도록 만들기 위해 사용된다
	bool Server_UseDoorKeySameOrLessThan(int32 KeyId);

	// 보스 몬스터 추가/제거
	// 구조적으로 플레이어 또한 보스가 될 수 있다
	// 주어진 캐릭터를 보스 몬스터로 등록한다
	void Server_AddBossCharacter(class ABotCharacter* NewBoss);

	// 주어진 캐릭터를 보스 몬스터 항목에서 찾아 제거하고, 성공 여부를 반환한다
	bool Server_RemoveBossCharacter(class ABotCharacter* OldBoss);

	// 레퍼런스 Getter
	const TArray<class AGameCharacter*>& Local_GetBossCharacters();

	// 게임 오버 상태를 수정한다
	void Server_SetGameOverTo(bool bNewGameOver);

protected:
	UFUNCTION()
	void OnRep_DungeonTimeLeft();
	void Local_OnDungeonTimeLeftUpdated();

	UFUNCTION()
	void OnRep_BossCharacters();
	void Local_UpdateBossState();

	UFUNCTION()
	void OnRep_DungeonDoorKeys();
	void Local_OnDoorKeysChanged();

	UFUNCTION()
	void OnRep_GameOver();
	void Local_OnGameOverUpdated();
#pragma endregion

#pragma region /** UI */
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDamageNumberWidget> DefaultDamageNumberWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTRGameStateHUD> DefaultGameStateHUDClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameOverWidget> GameOverWidgetClass = nullptr;

protected:
	// 사용 가능한 풀
	// TODO: WidgetClass에 따라 다른 풀을 사용하면 조금 더 효율적으로 관리할 수 있음 (현재는 풀링된 것과 위젯이 다를 경우 새로 생성함)
	TQueue<ALocalDamageNumber*> UsableLocalDamageNumberPool;

public:
	// GameState에서 관리하는 위젯들 (생성 주체와 관리 주체가 동일하다)
	UTRGameStateHUD* GameStateHUD = nullptr;
	class UGameOverWidget* GameOverWidgetInst = nullptr;

public:
	// NOTE: 풀
	ALocalDamageNumber* Local_DisplayDamageNumber(UWorld* World, TSubclassOf<class UDamageNumberWidget> WidgetClass, const FTransform& Transform, int32 DmgValue, bool bForceNewInstance);

	// 던전 문 키의 개수 반환
	int32 Host_GetDoorKeyCount() { return DungeonDoorKeys.Num(); }
#pragma endregion
};
