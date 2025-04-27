// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"

#include "UI/TRCrosshair.h"
#include "FPSHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API AFPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

public:
	// 사용할 크로스헤어 클래스
	// Level Persistent
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTRCrosshair> CrosshairClass = nullptr;

	// 현재 사용중인 크로스헤어 위젯 인스턴스
	// Level Persistent
	UPROPERTY(BlueprintReadWrite)
	class UTRCrosshair* Crosshair = nullptr;
};
