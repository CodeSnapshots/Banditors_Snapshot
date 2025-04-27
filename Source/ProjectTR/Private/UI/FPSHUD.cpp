// Copyright (C) 2024-2025 by Haguk Kim


#include "UI/FPSHUD.h"
#include "UI/TRCrosshair.h"

void AFPSHUD::BeginPlay()
{
	Super::BeginPlay();

	if (CrosshairClass)
	{
		Crosshair = CreateWidget<UTRCrosshair>(GetWorld(), CrosshairClass);
	}
	if (Crosshair)
	{
		Crosshair->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AFPSHUD::DrawHUD - Something went wrong, crosshair generation failed!"));
	}
}

void AFPSHUD::DrawHUD()
{
	Super::DrawHUD();
}
