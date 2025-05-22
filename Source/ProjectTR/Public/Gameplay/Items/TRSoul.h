// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Core/TRStructs.h"
#include "Items/BaseItem.h"
#include "Characters/FPSCharacter.h"
#include "TRSoul.generated.h"

UCLASS()
class PROJECTTR_API ATRSoul : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ATRSoul();

protected:
	virtual void BeginPlay() override;

protected:
	// NOTE: 현재로써 영혼 아이템의 ItemData를 사용한 복구는 서버에서만 처리되어도 문제가 없음
	virtual bool RestoreFromItemData(UItemData* Data) override;

public:
	// 이 영혼 아이템이 현재 새 플레이어 부활을 위해 사용될 수 있는지 여부
	// 필요한 정보가 누락되었거나,
	// 저장되어있는 플레이어 컨트롤러가 이미 AFPSCharacter 하위 인스턴스를 Possess중인 경우 false를 반환한다
	bool IsReadyToRespawnPlayer() const;

	class ATRPlayerController* Server_GetController() const { return Server_Controller; }
	void Server_SetController(class ATRPlayerController* Controller) { Server_Controller = Controller; }

protected:
	// 부활 대상 플레이어
	UPROPERTY(BlueprintReadOnly)
	class ATRPlayerController* Server_Controller = nullptr;

	// NOTE: 영혼석 자체는 인스턴스 정보를 저장하지 않음
	// PlayerState에 정보가 저장됨
};
