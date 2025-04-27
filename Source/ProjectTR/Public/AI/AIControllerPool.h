// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIControllerPool.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTTR_API UAIControllerPool : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIControllerPool();
	virtual void BeginPlay() override;

	// 인자로 주어진 봇에게 풀에서 컨트롤러 하나를 할당해 Possess한다
	class ABaseAIController* Animate(class ABotCharacter* Bot, bool bForceNewController = false);

	// 봇에 할당된 컨트롤러를 Unpossess해 오브젝트풀로 반환한다
	void Inanimate(class ABotCharacter* Bot);

	// 풀을 초기화한다
	void Initialize(int32 PoolSize);

	// 풀을 파괴하고, 풀에서 생성한 자원을 모두 회수한다
	void DestroyPool();

protected:
	// AIControllerClass를 기반으로 새 AI 컨트롤러를 생성한다
	// 비활성화 상태로 생성한다
	class ABaseAIController* CreateNewBaseAIController();

protected:
	// AI 풀에 사용할 컨트롤러 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABaseAIController> AIControllerClass;

private:
	// 오브젝트 풀
	TQueue<class ABaseAIController*> Pool;
};
