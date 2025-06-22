// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"
#include "Components/BoxComponent.h"

#include "Gameplay/DungeonActors/DungeonActor.h"
#include "BreakableActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ABreakableActor : public ADungeonActor
{
	GENERATED_BODY()
	
public:
	ABreakableActor();
	virtual void BeginPlay() override;

	// 파괴 시 유일한 진입점; Chaos의 피직스 기반 breaking은 사용하지 않는다
	virtual void Server_ProcessCrumbling(const FVector& CrumblingVelocity);

	// 충돌한 액터가 이 액터를 파괴 가능한지 반환
	// 이미 충돌된 액터들만 입력으로 주어졌을 때 유효한 결과를 반환
	virtual bool CanBreak(AActor* CollidedActor);

protected:
/* Collision */
	// 객체에 따라 오버랩 혹은 히트 모두 사용해 로직에 진입한다
	UFUNCTION()
	void Server_OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void Server_OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 공통 로직
	void Server_OnCollision(AActor* OtherActor);

/* Destruction */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ProcessCrumbling(FVector CrumblingVelocity);

	void Local_ProcessCrumbling(FVector Velocity);

/* Lifecycle */
	UFUNCTION()
	void Server_OnRemoval(const FChaosRemovalEvent& RemovalEvent);

	UFUNCTION()
	void DestroySelf();

public:
	UPROPERTY(EditDefaultsOnly)
	UGeometryCollectionComponent* GeometryComponent;

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly)
	float MinCrumbleSpeed = 250.0f;

	UPROPERTY(EditDefaultsOnly)
	float MaxCrumbleSpeed = 350.0f;
};
