// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/DungeonActors/MuzzleTriggeredActor.h"
#include "MuzzleTouchTriggeredActor.generated.h"

/**
 * 머즐 및 오버랩으로 모두 트리거가 가능한 액터를 나타낸다
 */
UCLASS()
class PROJECTTR_API AMuzzleTouchTriggeredActor : public AMuzzleTriggeredActor
{
	GENERATED_BODY()
	
public:
	AMuzzleTouchTriggeredActor();
	virtual void BeginPlay() override;

protected:
	// 바인딩할 함수
	UFUNCTION()
	void Server_OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OverlapTrigger(AActor* TriggeredBy);
	virtual void OnOverlapTriggered(AActor* TriggeredBy);

protected:
	// NOTE: 기본적으로 오버랩을 전부 감지는 하되, 오버랩 발생 시 트리거 여부는 여기서 별도로 조정해야 한다
	UPROPERTY(EditDefaultsOnly)
	bool bAllowPlayerOverlap = false;

	UPROPERTY(EditDefaultsOnly)
	bool bAllowBotOverlap = false;

	UPROPERTY(EditDefaultsOnly)
	bool bAllowItemOverlap = false;
};
