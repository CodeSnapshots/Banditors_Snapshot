// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"

#include "DungeonActors/DungeonActor.h"
#include "CharacterTriggerVolume.generated.h"

/**
 * NOTE: 이 클래스를 블루프린트에서 재상속하는 것은 가급적 지양해야 한다
 * 볼륨은 팩토리에 의해 생성되기 때문이다
 */
UCLASS()
class PROJECTTR_API ACharacterTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()
	
public:
	ACharacterTriggerVolume();
	virtual void BeginPlay() override;

	// 오버랩 시 처리 로직
	UFUNCTION()
	void Server_OverlapEntered(class AActor* OverlappedActor, class AActor* OtherActor);

	// 오버랩 해제 시 처리 로직
	UFUNCTION()
	void Server_OverlapLeft(class AActor* OverlappedActor, class AActor* OtherActor);

	// 현재 이 볼륨에 모든 플레이어들이 오버랩 된 상태인지 여부
	UFUNCTION()
	bool IsAllPlayerOverlapped();

	/* Setters */
	UFUNCTION()
	void SetPairActor(class ADungeonActor* Actor) { Pair = Actor; }

protected:
	// 캐릭터 오버랩 로직
	void Server_OnCharacterOverlapBegin(class AGameCharacter* Target);
	void Server_OnCharacterOverlapEnd(class AGameCharacter* Target);

	// 오버랩 커스텀 로직
	virtual void ProcessCharacterOverlap(class AGameCharacter* Target);
	virtual void ProcessPlayerOverlap(class AFPSCharacter* Player, bool bAllPlayersOverlapped);
	virtual void ProcessAllPlayersOverlap();

protected:
	// 현재 오버랩된 플레이어 수
	int OverlappedPlayerCount = 0;

	// 이 볼륨이 영향을 미칠 대상 액터
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	TWeakObjectPtr<ADungeonActor> Pair;

	// Pair를 언제 트리거 할지 조정
	// 캐릭터 오버랩 발생 시 트리거
	UPROPERTY(EditAnywhere)
	bool bTriggerPairWhenCharacterOverlap = false;

	// 플레이어 오버랩 발생 시 트리거
	UPROPERTY(EditAnywhere)
	bool bTriggerPairWhenPlayerOverlap = false;

	// 모든 플레이어가 오버랩 할 경우 트리거
	// bTriggerPairWhenPlayerOverlap가 true인 경우 무시됨
	UPROPERTY(EditAnywhere)
	bool bTriggerPairWhenAllPlayerOverlap = false;

	// 최초 1회만 트리거 할 지 여부
	// 주의: 이 값은 페어를 트리거하는 것에 대한 처리 여부이며, 오버랩이 발생한 횟수와는 다르다
	UPROPERTY(EditAnywhere)
	bool bTriggerPairOnceOnly = false;
	bool bHasTriggeredPairOnceOrMore = false;
};
