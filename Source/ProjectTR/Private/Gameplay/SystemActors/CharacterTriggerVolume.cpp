// Copyright (C) 2024-2025 by Haguk Kim


#include "SystemActors/CharacterTriggerVolume.h"
#include "Components/BrushComponent.h"

#include "Core/ProjectTRGameModeBase.h"
#include "Core/TRMacros.h"
#include "Characters/FPSCharacter.h"

ACharacterTriggerVolume::ACharacterTriggerVolume()
{
    // 콜리전
    UBrushComponent* BrushComp = GetBrushComponent();
    if (BrushComp)
    {
        BrushComp->SetCollisionProfileName(TEXT("CharacterTriggerVolume"));
    }
}

void ACharacterTriggerVolume::BeginPlay()
{
    Super::BeginPlay();

    // 서버의 경우에만 델리게이트 등록
    if (HasAuthority())
    {
        OnActorBeginOverlap.AddDynamic(this, &ACharacterTriggerVolume::Server_OverlapEntered);
        OnActorEndOverlap.AddDynamic(this, &ACharacterTriggerVolume::Server_OverlapLeft);
    }
}

void ACharacterTriggerVolume::Server_OverlapEntered(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!HasAuthority()) return;
    if (OtherActor && (OtherActor != this))
    {
        AGameCharacter* Target = Cast<AGameCharacter>(OtherActor);
        if (IsValid(Target))
        {
            Server_OnCharacterOverlapBegin(Target);
        }
    }
}

void ACharacterTriggerVolume::Server_OverlapLeft(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!HasAuthority()) return;
    if (OtherActor && (OtherActor != this))
    {
        AGameCharacter* Target = Cast<AGameCharacter>(OtherActor);
        if (IsValid(Target))
        {
            Server_OnCharacterOverlapEnd(Target);
        }
    }
}

bool ACharacterTriggerVolume::IsAllPlayerOverlapped()
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("IsAllPlayerOverlapped - This is not intended to be called from non-authoritative host."));
        return false;
    }

    UWorld* World = GetWorld();
    if (World)
    {
        AProjectTRGameModeBase* TRGameMode = Cast<AProjectTRGameModeBase>(World->GetAuthGameMode());
        if (TRGameMode)
        {
            // 살아있는 플레이어 캐릭터들의 목록을 가져와 그 크기를 구한다
            int PlayerCount = TRGameMode->GetInGamePlayerCharacters(true).Num();
            if (PlayerCount <= OverlappedPlayerCount)
            {
                if (PlayerCount < OverlappedPlayerCount)
                {
                    UE_LOG(LogTemp, Error, TEXT("IsAllPlayerOverlapped - OverlappedPlayerCount is larger than the actual player count. This should not happen."));
                }
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    UE_LOG(LogTemp, Error, TEXT("IsAllPlayerOverlapped - Something unexpected happen. Please check."));
    return false;
}

void ACharacterTriggerVolume::Server_OnCharacterOverlapBegin(AGameCharacter* Target)
{
    if (!IsValid(Target)) return;
    if (bTriggerPairOnceOnly && bHasTriggeredPairOnceOrMore) return;
    
    // 이번 오버랩으로 인해 모든 플레이어가 오버랩되었는지 여부
    // 즉, 이미 모든 플레이어가 오버랩된 상황에서 논플레이어 캐릭터가 오버랩될 경우 이 값은 false이다
    bool bAllPlayersOverlappedNow = false;
    AFPSCharacter* Player = Cast<AFPSCharacter>(Target);

    // 트리거 처리
    if (IsValid(Player))
    {
        ++OverlappedPlayerCount;
        bAllPlayersOverlappedNow = IsAllPlayerOverlapped();

        if (bTriggerPairWhenPlayerOverlap)
        {
            if (Pair.IsValid())
            {
                Pair->TriggerThis();
            }
            bHasTriggeredPairOnceOrMore = true;
        }
        else if (bTriggerPairWhenAllPlayerOverlap && bAllPlayersOverlappedNow)
        {
            if (Pair.IsValid())
            {
                Pair->TriggerThis();
            }
            bHasTriggeredPairOnceOrMore = true;
        }
    }
    else
    {
        if (bTriggerPairWhenCharacterOverlap)
        {
            if (Pair.IsValid())
            {
                Pair->TriggerThis();
            }
            bHasTriggeredPairOnceOrMore = true;
        }
    }

    // 로직 처리
    ProcessCharacterOverlap(Target);
    if (IsValid(Player))
    {
        ProcessPlayerOverlap(Player, bAllPlayersOverlappedNow);
        if (bAllPlayersOverlappedNow)
        {
            ProcessAllPlayersOverlap();
        }
    }
}

void ACharacterTriggerVolume::Server_OnCharacterOverlapEnd(AGameCharacter* Target)
{
    if (!IsValid(Target)) return;
    AFPSCharacter* Player = Cast<AFPSCharacter>(Target);
    if (IsValid(Player))
    {
        --OverlappedPlayerCount;
        if (OverlappedPlayerCount < 0)
        {
            UE_LOG(LogTemp, Error, TEXT("Server_OnPlayerOverlapEnd - OverlappedPlayerCount should never go under zero!"));
            OverlappedPlayerCount = 0;
        }
    }
}

void ACharacterTriggerVolume::ProcessCharacterOverlap(AGameCharacter* Target)
{
    // 필요 시 오버라이드
}

void ACharacterTriggerVolume::ProcessPlayerOverlap(AFPSCharacter* Player, bool bAllPlayersOverlapped)
{
    // 필요 시 오버라이드
}

void ACharacterTriggerVolume::ProcessAllPlayersOverlap()
{
    // 필요 시 오버라이드
}
