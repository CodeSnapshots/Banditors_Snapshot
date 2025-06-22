// Copyright (C) 2024-2025 by Haguk Kim


#include "TRExplosion.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

#include "Core/TRUtils.h"
#include "Core/TRCVar.h"
#include "Characters/GameCharacter.h"
#include "Characters/FPSCharacter.h"
#include "Damage/TRDamageType.h"

ATRExplosion::ATRExplosion()
{
	PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    ExplRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("ExplRoot"));
    check(ExplRootComp != nullptr);
    SetRootComponent(ExplRootComp);

    // 기본 VFX 컨픽 바인딩
    if (!GunFxConfig)
    {
        static ConstructorHelpers::FObjectFinder<UGunFxConfig> FXFinder(TEXT(ASSET_DEFAULT_GUN_FX));
        if (FXFinder.Succeeded())
        {
            GunFxConfig = FXFinder.Object;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ATRExplosion - Unable to find default FX config asset!"));
        }
    }
}

void ATRExplosion::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (HasAuthority())
    {
        if (GunFxConfig)
        {
            ExplosionVFX = GunFxConfig->SearchNiagaraFromEnum(ExplosionInfo.ExplosionVFXEnum);
        }
        float ScaleScalar = (ExplosionInfo.ExplosionRadius * ExplosionInfo.VFXRadiusConstant);
        ExplosionVFXScale = FVector(ScaleScalar, ScaleScalar, ScaleScalar);
    }
}

void ATRExplosion::BeginPlay()
{
	Super::BeginPlay();
	
    if (HasAuthority() && ExplosionInfo.bExplodeOnBeginPlay)
    {
        Server_Explode();
    }
}

void ATRExplosion::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ATRExplosion::Server_Explode()
{
    Multicast_Explode();

    TArray<AActor*> Targets;
    TArray<AActor*> Ignored = { this };

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectQuery;
    for (const TEnumAsByte<ECollisionChannel>& ECC : ExplosionInfo.ExplosionTargetType)
    {
        TEnumAsByte<EObjectTypeQuery> QueryType = UEngineTypes::ConvertToObjectType(ECC);
        ObjectQuery.Add(QueryType);
    }

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        this->GetActorLocation(),
        ExplosionInfo.ExplosionRadius,
        ObjectQuery,
        nullptr,
        Ignored,
        Targets
    );

    for (AActor* Target : Targets)
    {
        AGameCharacter* GameCharacter = Cast<AGameCharacter>(Target);
        ABaseItem* GameItem = Cast<ABaseItem>(Target);

        // 폭발 지점과 대상 사이에 장애물이 없는지 확인
        FVector BlockCheckStartLoc = GetActorLocation() + (CollisionNormal.GetSafeNormal() * BlockCheckDeltaSize);
        if (Target && !IsBlocked(BlockCheckStartLoc, Target->GetActorLocation()))
        {
            if (GameCharacter)
            {
                if (ExplosionInfo.bApplyDamageOnExplosion)
                {
                    FHitResult ExplosionHitRes;
                    FVector TargetLoc = GameCharacter->GetActorLocation();
                    FVector ExplosionLoc = this->GetActorLocation();
                    ExplosionHitRes.ImpactNormal = ExplosionLoc - TargetLoc;
                    ExplosionHitRes.ImpactPoint = TargetLoc;
                    ExplosionHitRes.TraceStart = ExplosionLoc;
                    ExplosionHitRes.Location = TargetLoc;
                    // 필요 시 나머지 정보 기입

                    UGameplayStatics::ApplyPointDamage(
                        GameCharacter,
                        GetDamageOnTarget(GameCharacter),
                        GameCharacter->GetActorLocation() - ExplosionHitRes.TraceStart,
                        ExplosionHitRes,
                        GetInstigatorController(),
                        GetInstigator(),
                        ExplosionInfo.ExplosionDamageType
                    );
                }

                const FVector& KnockbackDirection = (GameCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();

                // Linear하게 넉백 감소
                float ExplRadius = FMath::Max(ExplosionInfo.ExplosionRadius, 1.f); // divide by zero 방지
                float KnockbackStrength = FMath::Lerp<float>(
                    0.f,
                    ExplosionInfo.BaseImpactStrength,
                    (ExplRadius - FMath::Min(GetDistanceTo(GameCharacter), ExplRadius)) / ExplRadius
                );

                // 자가 넉백 배수
                if (GameCharacter == GetInstigator())
                {
                    KnockbackStrength *= ExplosionInfo.InstigatorImpactMult;
                }

                GameCharacter->LaunchCharacter(
                    KnockbackDirection * KnockbackStrength * TR_EXPL_KNOCKBACK_CONST_FOR_CHARACTERS,
                    false,
                    false
                );

#if WITH_EDITOR
                if (CVarShowDebugShapes.GetValueOnGameThread())
                {
                    DrawDebugLine(GetWorld(), this->GetActorLocation(), GameCharacter->GetActorLocation(), FColor::Blue, false, 10.0f);
                }
#endif
            }
            else if (GameItem)
            {
                UPrimitiveComponent* PhysComp = GameItem->GetPhysComponent();
                if (PhysComp && ExplosionInfo.bApplyImpactOnExplosion && ExplosionInfo.ExplosionTargetType.Contains(PhysComp->GetCollisionObjectType()))
                {
                    PhysComp->AddRadialImpulse(
                        GetActorLocation(), 
                        ExplosionInfo.ExplosionRadius, 
                        ExplosionInfo.BaseImpactStrength,
                        ERadialImpulseFalloff::RIF_Linear,
                        true
                    );
                }

#if WITH_EDITOR
                if (CVarShowDebugShapes.GetValueOnGameThread())
                {
                    DrawDebugLine(GetWorld(), this->GetActorLocation(), GameItem->GetActorLocation(), FColor::Green, false, 10.0f);
                }
#endif
            }
            else
            {
                // NOTE: 현재로는 캐릭터와 아이템에만 폭발 로직을 적용
            }
        }
    }

#if WITH_EDITOR
    if (CVarShowDebugShapes.GetValueOnGameThread())
    {
        DrawDebugSphere(GetWorld(), this->GetActorLocation(), ExplosionInfo.ExplosionRadius, 8, FColor::Yellow, false, 10.0f);
    }
#endif

    if (ExplosionInfo.bDestroyAfterExplosion)
    {
        Destroy();
    }
}

void ATRExplosion::Multicast_Explode_Implementation()
{
    Local_PlayExplosionVFX();
}

float ATRExplosion::GetDamageOnTarget(class AGameCharacter* Target)
{
    if (!Target || ExplosionInfo.ExplosionRadius <= 0) return 0;
    float Damage = FMath::Lerp(
        ExplosionInfo.BaseDamage, // 최대 데미지 (point blank)
        ExplosionInfo.MinExplosionMultiplier * ExplosionInfo.BaseDamage, // 최소 데미지
        FMath::Min(1.0f, GetDistanceTo(Target) / ExplosionInfo.ExplosionRadius) // 최소 데미지보다 적은 데미지를 받을 수 없다
    );
    
    APawn* ExplInstigator = GetInstigator();
    if (ExplInstigator && (Target == ExplInstigator || (Target->IsA<AFPSCharacter>() && ExplInstigator->IsA<AFPSCharacter>())))
    {
        Damage *= ExplosionInfo.DmgMultOnExplInstigator;
    }
    return Damage;
}

bool ATRExplosion::IsBlocked(FVector StartLocation, FVector TargetLocation)
{
    FCollisionQueryParams TraceParams(FName(TEXT("ExplosionLinetrace")), false, this);
    TraceParams.bReturnPhysicalMaterial = false;
    TraceParams.AddIgnoredActor(this);
    FCollisionResponseParams ColParams;

    // 블로킹 대상 채널 설정
    ColParams.CollisionResponse.SetAllChannels(ECR_Ignore);
    for (const ECollisionChannel& BlockingChannel : ExplosionInfo.ExplosionBlockedByType)
    {
        ColParams.CollisionResponse.SetResponse(BlockingChannel, ECR_Block);
    }

    FHitResult HitRes;
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitRes,
        StartLocation,
        TargetLocation,
        ECC_Explosion,
        TraceParams,
        ColParams
    );

    if (bHit)
    {
#if WITH_EDITOR
        if (CVarShowDebugShapes.GetValueOnGameThread())
        {
            DrawDebugLine(GetWorld(), StartLocation, TargetLocation, FColor::Red, false, 10.0f);
            DrawDebugPoint(GetWorld(), HitRes.Location, 10.0f, FColor::Yellow, false, 10.0f);
        }
#endif
        return true;
    }
    return false;
}

void ATRExplosion::Local_PlayExplosionVFX()
{
    if (!ExplosionVFX) return;
    UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        ExplosionVFX,
        GetActorLocation(),
        GetActorRotation(),
        ExplosionVFXScale,
        true, 
        true,
        ENCPoolMethod::AutoRelease,
        true
    );
}

void ATRExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

