// Copyright (C) 2024-2025 by Haguk Kim


#include "DungeonActors/TRAltar.h"
#include "Components/BoxComponent.h"

#include "Core/TRMacros.h"
#include "Core/TRCVar.h"
#include "Core/ProjectTRGameModeBase.h"
#include "Core/TRPlayerController.h"
#include "Characters/TRPlayerState.h"
#include "Items/TRToken.h"
#include "Items/TRSoul.h"
#include "Items/BaseItem.h"
#include "Items/GunItem.h"

ATRAltar::ATRAltar()
{
	PrimaryActorTick.bCanEverTick = false;

    if (!DetectionComponent)
    {
        DetectionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionComponent"));
    }
    DetectionComponent->SetupAttachment(RootComponent);
    DetectionComponent->SetCollisionProfileName(TEXT("AltarDetector"));

    if (!SpawnPointComponent)
    {
        SpawnPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPointComponent"));
    }
    SpawnPointComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATRAltar::BeginPlay()
{
	Super::BeginPlay();

    // 서버의 경우만 함수 바인딩
    if (HasAuthority())
    {
        DetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATRAltar::Server_OnAltarOverlap);
    }
}

void ATRAltar::Server_OnAltarOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;
    if (bAltarDestroyed) return;

    if (IsValid(OtherActor) && (OtherActor != this))
    {
        ABaseItem* CollidedItem = Cast<ABaseItem>(OtherActor);
        if (IsValid(CollidedItem))
        {
            if (Server_OnItemDetection(CollidedItem))
            {
                // 아이템 소비 성공 시 후처리
                Server_PostAltarUsage();
            }

#if WITH_EDITOR
            if (CVarShowScreenDebugMsgs.GetValueOnGameThread())
            {
                FString ItemName = CollidedItem->GetName();
                TR_PRINT_ARGS("Item Dropped: %s", *ItemName);
            }
#endif
        }
    }
}

bool ATRAltar::Server_OnItemDetection(ABaseItem* Item)
{
    if (!HasAuthority()) return false;

    ATRToken* Token = Cast<ATRToken>(Item);
    if (Token)
    {
        if (Server_OnTokenDetection(Token))
        {
            CurrDestructionChance += AddDestructionChance_Token;
            return true;
        }
    }

    ATRSoul* Soul = Cast<ATRSoul>(Item);
    if (Soul)
    {
        if (Server_OnSoulDetection(Soul))
        {
            CurrDestructionChance += AddDestructionChance_Soul;
            return true;
        }
    }

    AGunItem* Gun = Cast<AGunItem>(Item);
    if (Gun)
    {
        return Server_OnGunDetection(Gun);
    }

    // 그 외의 아이템의 경우 별도 로직 없음
    return false;
}

bool ATRAltar::Server_OnTokenDetection(ATRToken* Token)
{
    if (!HasAuthority()) return false;

    AProjectTRGameModeBase* GameMode = Cast<AProjectTRGameModeBase>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
#if WITH_EDITOR
        if (CVarShowScreenDebugMsgs.GetValueOnGameThread())
        {
            TR_PRINT_ARGS("Token tier: %d", Token->GetTier());
        }
#endif
        GameMode->SpawnRandomizedGunItem(GetWorld(), GetSpawnLocation(), GetSpawnRotation(), FActorSpawnParameters(), Token->GetTier());
        Token->Destroy();
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Server_OnTokenDetection - Invalid game mode!"));
    }
    return false;
}

bool ATRAltar::Server_OnGunDetection(AGunItem* Gun)
{
    if (!HasAuthority()) return false;

    // 필요 시 작성
    return false;
}

bool ATRAltar::Server_OnSoulDetection(ATRSoul* Soul)
{
    if (!HasAuthority()) return false;

    AProjectTRGameModeBase* GameMode = Cast<AProjectTRGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("Server_OnSoulDetection - Invalid game mode!"));
        return false;
    }
    if (!Soul->IsReadyToRespawnPlayer() || !Soul->Server_GetController())
    {
        UE_LOG(LogTemp, Error, TEXT("Server_OnSoulDetection - Invalid soul item!"));
        return false;
    }
    ATRPlayerState* TRPS = Soul->Server_GetController()->GetPlayerState<ATRPlayerState>();
    if (!TRPS)
    {
        UE_LOG(LogTemp, Error, TEXT("Server_OnSoulDetection - Invalid player state!"));
        return false;
    }

    TPair<TSubclassOf<AFPSCharacter>, FGameCharacterInstanceData*> CachedInstData = TRPS->Server_GetCachedPlayerInstanceData();
    GameMode->RespawnPlayer(Soul->Server_GetController(), FTransform(GetSpawnRotation(), GetSpawnLocation(), FVector(1, 1, 1)), CachedInstData.Get<0>(), *CachedInstData.Get<1>(), true/*체력 회복*/);
    Soul->Destroy();
    return true;
}

FVector ATRAltar::GetSpawnLocation()
{
    FVector SpawnLocation = this->GetActorLocation();
    if (SpawnPointComponent)
    {
        SpawnLocation = SpawnPointComponent->GetComponentLocation();
    }
    return SpawnLocation;
}

FRotator ATRAltar::GetSpawnRotation()
{
    FRotator SpawnRotation = this->GetActorRotation();
    if (SpawnPointComponent)
    {
        SpawnRotation = SpawnPointComponent->GetComponentRotation();
    }
    return SpawnRotation;
}

void ATRAltar::Server_PostAltarUsage()
{
    if (!HasAuthority()) return;
    if (FMath::FRand() <= CurrDestructionChance)
    {
        Server_DestroyAltar();
    }
}

void ATRAltar::Server_DestroyAltar()
{
    if (!HasAuthority()) return;
    Multicast_DestroyAltar();

    bAltarDestroyed = true;
}

void ATRAltar::Multicast_DestroyAltar_Implementation()
{
    Local_OnAltarDestruction();
}

