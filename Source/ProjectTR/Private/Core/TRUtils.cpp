// Copyright (C) 2024-2025 by Haguk Kim


#include "Core/TRUtils.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Characters/GameCharacter.h"
#include "Characters/FPSCharacter.h"
#include "Characters/BotCharacter.h"
#include "Items/BaseItem.h"
#include "Items/GunItem.h"
#include "Items/GunParts/GunPartComponent.h"
#include "DungeonActors/MuzzleTriggeredActor.h"


FString TRUtils::TimeSecondsToString(float InSeconds)
{
    const TCHAR* NegativeModifier = InSeconds < 0.f ? TEXT("-") : TEXT("");
    InSeconds = FMath::Abs(InSeconds);
    const int32 NumMinutes = FMath::FloorToInt(InSeconds / 60.f);
    const int32 NumSeconds = FMath::FloorToInt(InSeconds - (NumMinutes * 60.f));
    return FString::Printf(TEXT("%s%02d:%02d"), NegativeModifier, NumMinutes, NumSeconds);
}

float TRUtils::GetFallOffMultOfDist(float Distance, float MaxDistDmgMult, float MaxFallOffDist, float MinFallOffDist)
{
    if (Distance <= MinFallOffDist) return 1.0f;
    float DistAlpha = FMath::Min(Distance, MaxFallOffDist);
    return FMath::Lerp(1.0f, MaxDistDmgMult, (DistAlpha - MinFallOffDist) / (MaxFallOffDist - MinFallOffDist));
}

const TArray<FDropItem> TRUtils::SelectDropItems(const TArray<FDropItem>& Candidates)
{
	TArray<FDropItem> Result;
	for (const FDropItem& Loot : Candidates)
	{
		float DropChance = FMath::Clamp(Loot.DropRate, 0, 1);
		if (FMath::FRand() <= Loot.DropRate)
		{
			Result.Add(Loot);
		}
	}
	return Result;
}

const TArray<FDropItem> TRUtils::FilterAndSelectCandidates(const TArray<FUnfilteredDropItem>& Candidates, int32 DungeonDepth, int32 Count)
{
    TArray<const FDropItem*> Selected;
    TArray<float> Weights;
    for (const FUnfilteredDropItem& Cand : Candidates)
    {
        if (Cand.MaxDepth >= DungeonDepth && DungeonDepth >= Cand.MinDepth)
        {
            Selected.Add(&Cand.DropItem);
            Weights.Add(Cand.SelectWeight);
        }
    }

    TArray<FDropItem> Result;
    for (int32 i = 0; i < Count; ++i)
    {
        Result.Add(*TRUtils::GetRandomElementByWeight(Selected, Weights));
    }
    return Result;
}

TSubclassOf<ABotCharacter> TRUtils::SelectSpawnMonster(TArray<FSpawnableMonsterData>& Candidates)
{
    if (!Candidates.IsEmpty())
    {
        TArray<TSubclassOf<ABotCharacter>*> Elements;
        TArray<float> Weights;
        for (int32 Idx = 0; Idx < Candidates.Num(); ++Idx)
        {
            Elements.Add(&Candidates[Idx].BotClass);
            Weights.Add(Candidates[Idx].SpawnRate);
        }
        TSubclassOf<ABotCharacter>* Chosen = GetRandomElementByWeight<TSubclassOf<ABotCharacter>>(Elements, Weights);
        return *Chosen;
    }
    return nullptr;
}

float TRUtils::FindLargestElement(const FVector& Vector)
{
    return FMath::Max(FMath::Max(Vector.X, Vector.Y), Vector.Z);
}

FVector TRUtils::FindRandomNavigationPoint(UWorld* World, const FVector& Center, float Radius)
{
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("FindRandomNavigationPoint - Invalid world!"));
        return FVector::ZeroVector;
    }

    FVector FoundLocation = Center;
    FNavLocation RandomPoint;
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
    if (NavSys && NavSys->GetRandomPointInNavigableRadius(Center, Radius, RandomPoint))
    {
        FoundLocation = RandomPoint.Location;
    }
    return FoundLocation;
}

bool TRUtils::GetMovablePointNearActor(UWorld* World, AActor* Target, FName CollisionProfileName, float VertDistLimit, float HorDistLimit, FVector& out_Point)
{
    if (!World || !Target) return false;

    // XY좌표 기준으로 바닥을 찾아 네브포인트를 찾는다
    FHitResult GroundHit;
    FVector TargetLocation = Target->GetActorLocation();
    FVector LineEnd = TargetLocation;
    LineEnd.Z -= VertDistLimit;

    FCollisionQueryParams GroundColParams;
    GroundColParams.AddIgnoredActor(Target); // 타깃은 무시

    World->LineTraceSingleByProfile(GroundHit, TargetLocation, LineEnd, CollisionProfileName, GroundColParams);
    out_Point = TRUtils::FindRandomNavigationPoint(World, Target->GetActorLocation(), HorDistLimit);

    return true;
}

bool TRUtils::IsAllyWith(AGameCharacter* CharA, AGameCharacter* CharB)
{
    if (!CharA || !CharB) return false;
    if (CharA == CharB) return true;
    if (CharA->IsA<AFPSCharacter>() && CharB->IsA<AFPSCharacter>())
    {
        return true;
    }
    else if (CharA->IsA<ABotCharacter>() && CharB->IsA<ABotCharacter>())
    {
        return true;
    }
    return false;
}

void TRUtils::ApplyStatusEffectsOnTarget(const TArray<FStatEffectGenInfo>& Effects, AGameCharacter* Target, AGameCharacter* Applier)
{
    if (!IsValid(Target))
    {
        UE_LOG(LogTemp, Error, TEXT("ApplyStatusEffectsOnTarget - Invalid target"));
        return;
    }
    for (const FStatEffectGenInfo& Effect : Effects)
    {
        Target->Server_GenerateAndAddStatEffect(Effect, Applier);
    }
}

bool TRUtils::GetRandomSpawnLocation(const TArray<class ASpawnPoint*>& SpawnPoints, ESpawnPointType SearchType, FVector& out_SpawnLocation)
{
    TArray<ASpawnPoint*> ValidCandidates;
    for (ASpawnPoint* SpawnPoint : SpawnPoints)
    {
        if (SpawnPoint && SpawnPoint->SpawnType == SearchType)
        {
            ValidCandidates.Add(SpawnPoint);
        }
    }
    if (!ValidCandidates.IsEmpty())
    {
        ASpawnPoint* ChosenPoint = ValidCandidates[FMath::Rand() % ValidCandidates.Num()];
        if (ChosenPoint)
        {
            out_SpawnLocation = ChosenPoint->GetRandomSpawnLocation();
            return true;
        }
    }
    out_SpawnLocation = FVector::ZeroVector;
    return false;
}

void TRUtils::OptimizePrimitiveComp(UPrimitiveComponent* Component, bool bCastShadows, bool bDisableLights)
{
    if (!Component) return;
    Component->SetCastShadow(bCastShadows);
    if (bDisableLights)
    {
        Component->SetAffectDistanceFieldLighting(false);
        Component->SetAffectDynamicIndirectLighting(false);
        Component->SetLightingChannels(false, false, false);
    }
}

TSet<UPrimitiveComponent*> TRUtils::GetOutlineMeshesFromActor(AActor* Target, bool bIgnoreCharacters)
{
    TSet<UPrimitiveComponent*> TargetComps;
    if (Target->IsA<ABaseItem>())
    {
        if (Target->IsA<AGunItem>())
        {
            AGunItem* GunTarget = Cast<AGunItem>(Target);
            for (UGunPartComponent* GunPart : GunTarget->GetGunParts())
            {
                if (GunPart)
                {
                    UMeshComponent* GunMesh = GunPart->GetMeshComp();
                    if (GunMesh) TargetComps.Add(GunMesh);
                }
            }
        }
        else
        {
            TargetComps.Add(Cast<ABaseItem>(Target)->GetMeshComponent());
        }
    }
    else if (Target->IsA<AMuzzleTriggeredActor>())
    {
        TargetComps.Add(Cast<AMuzzleTriggeredActor>(Target)->MeshComponent);
    }
    else if (!bIgnoreCharacters && Target->IsA<AGameCharacter>())
    {
        TargetComps.Add(Cast<AGameCharacter>(Target)->GetMesh());
    }
    return TargetComps;
}

int32 TRUtils::GetOutlineStencilValueFromActor(AActor* Target)
{
    // 매터리얼 if 노드 숫자 감소를 위해 색상은 두 가지만 사용
    if (Target->IsA<ABaseItem>())
    {
        return STENCIL_WHITE;
    }
    else if (Target->IsA<AMuzzleTriggeredActor>())
    {
        return STENCIL_WHITE;
    }
    else if (Target->IsA<AGameCharacter>())
    {
        if (Target->IsA<AFPSCharacter>())
        {
            return STENCIL_WHITE;
        }
        return STENCIL_RED;
    }
    return STENCIL_WHITE;
}

FVector TRUtils::GetDefaultDropLocation(AActor* Dropper)
{
    FVector DropLocation = FVector::ZeroVector;
    if (Dropper)
    {
        AGameCharacter* DropperChar = Cast<AGameCharacter>(Dropper);
        if (DropperChar)
        {
            DropLocation = DropperChar->GetHandPointInfo().Get<0>();
        }
        else
        {
            Dropper->GetActorLocation();
        }
    }
    return DropLocation;
}

FRotator TRUtils::GetDefaultDropRotation(AActor* Dropper)
{
    FRotator DropRotation = FRotator::ZeroRotator;
    if (Dropper)
    {
        APawn* DropperPawn = Cast<APawn>(Dropper);
        if (DropperPawn)
        {
            DropRotation = DropperPawn->GetControlRotation();
        }
        else
        {
            DropRotation = Dropper->GetActorRotation();
        }
    }
    return DropRotation;
}

ETierNiagaraReference TRUtils::GetNiagaraRefFromTier(EItemTier Tier, bool bLoop)
{
    switch (Tier)
    {
        case EItemTier::IT_TIER_UNSPECIFIED:
        {
            return ETierNiagaraReference::ENR_NULL;
        }
        case EItemTier::IT_TIER_NONE:
        {
            return ETierNiagaraReference::ENR_NULL;
        }
        case EItemTier::IT_TIER_T1:
        {
            return bLoop ? ETierNiagaraReference::ENR_TIER_1_LOOP : ETierNiagaraReference::ENR_TIER_1_SPAWN;
        }
        case EItemTier::IT_TIER_T2:
        {
            return bLoop ? ETierNiagaraReference::ENR_TIER_2_LOOP : ETierNiagaraReference::ENR_TIER_2_SPAWN;
        }
        case EItemTier::IT_TIER_T3:
        {
            return bLoop ? ETierNiagaraReference::ENR_TIER_3_LOOP : ETierNiagaraReference::ENR_TIER_3_SPAWN;
        }
        case EItemTier::IT_TIER_T4:
        {
            return bLoop ? ETierNiagaraReference::ENR_TIER_4_LOOP : ETierNiagaraReference::ENR_TIER_4_SPAWN;
        }
        case EItemTier::IT_TIER_T5:
        {
            return bLoop ? ETierNiagaraReference::ENR_TIER_5_LOOP : ETierNiagaraReference::ENR_TIER_5_SPAWN;
        }
        case EItemTier::IT_OBJECTIVE:
        {
            return bLoop ? ETierNiagaraReference::ENR_OBJECTIVE_LOOP : ETierNiagaraReference::ENR_OBJECTIVE_SPAWN;
        }
        default:
        {
            return ETierNiagaraReference::ENR_NULL;
        }
    }
    return ETierNiagaraReference::ENR_NULL;
}
