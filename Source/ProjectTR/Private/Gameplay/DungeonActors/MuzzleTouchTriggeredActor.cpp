// Copyright (C) 2025 by Haguk Kim


#include "Gameplay/DungeonActors/MuzzleTouchTriggeredActor.h"
#include "Components/BoxComponent.h"

#include "Characters/BotCharacter.h"
#include "Characters/FPSCharacter.h"
#include "Items/BaseItem.h"

AMuzzleTouchTriggeredActor::AMuzzleTouchTriggeredActor()
{
    TriggerBox->SetCollisionProfileName(TEXT("MuzzleOverlapActor"));
	TriggerBox->SetGenerateOverlapEvents(true);
}

void AMuzzleTouchTriggeredActor::BeginPlay()
{
    Super::BeginPlay();

    // 서버의 경우만 함수 바인딩
    if (HasAuthority())
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMuzzleTouchTriggeredActor::Server_OnOverlap);
    }
}

void AMuzzleTouchTriggeredActor::Server_OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bAllowPlayerOverlap && OtherActor->IsA<AFPSCharacter>())
    {
        OverlapTrigger(OtherActor);
    }
    else if (bAllowBotOverlap && OtherActor->IsA<ABotCharacter>())
    {
        OverlapTrigger(OtherActor);
    }
    else if (bAllowItemOverlap && OtherActor->IsA<ABaseItem>())
    {
        OverlapTrigger(OtherActor);
    }
}

void AMuzzleTouchTriggeredActor::OverlapTrigger(AActor* TriggeredBy)
{
    OnOverlapTriggered(TriggeredBy);
}

void AMuzzleTouchTriggeredActor::OnOverlapTriggered(AActor* TriggeredBy)
{
    // 필요 시 오버라이드
}
