// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DungeonActors/DungeonActor.h"
#include "SpikeTrap.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ASpikeTrap : public ADungeonActor
{
	GENERATED_BODY()
	
public:
	ASpikeTrap();
	virtual void BeginPlay() override;
	virtual void OnTriggered() override;

	UFUNCTION()
	void Server_OverlapEntered(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void Server_OverlapLeft(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void OnBoxCollision(AActor* Target);

	void ApplyTrapDamage(class AGameCharacter* Target);

protected:
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* CollisionBox = nullptr;

	UPROPERTY(EditAnywhere)
	float TrapDamage = 0.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTRDamageType> DamageType;

	// 봇에게 데미지를 줄지 여부
	UPROPERTY(EditAnywhere)
	bool bDamageOnlyPlayers = false;

	// 트리거 호출이 된 적 있는지 여부
	bool bTrapTriggered = false;
};
