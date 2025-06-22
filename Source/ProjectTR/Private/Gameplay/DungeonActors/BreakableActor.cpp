// Copyright (C) 2025 by Haguk Kim


#include "Gameplay/DungeonActors/BreakableActor.h"

#include "Characters/FPSCharacter.h"
#include "Core/TRMacros.h"

ABreakableActor::ABreakableActor()
{
    if (RootComponent)
    {
        RootComponent->SetCanEverAffectNavigation(false);
    }

    // 별도의 콜리전 사용
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionComponent->SetupAttachment(RootComponent);
    CollisionComponent->SetCollisionProfileName("TRBreakableCollision");
    CollisionComponent->SetSimulatePhysics(false); // 피직스 콜리전만 사용
    CollisionComponent->SetEnableGravity(false);
    CollisionComponent->SetCanEverAffectNavigation(false);

    CollisionComponent->SetGenerateOverlapEvents(true); // 블록과 오버랩 모두 사용

    // 지오메트리
    GeometryComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryComp"));
    GeometryComponent->SetupAttachment(CollisionComponent);

    // 파괴 플로우를 더 안전하게 제어하기 위해, 직접 파괴 진입점을 통제한다
    GeometryComponent->SetSimulatePhysics(false); // 초기에는 피직스를 해제한 채로 시작
    GeometryComponent->SetEnableGravity(true); // NOTE: UE5.5 이하에서는 런타임에 중력 변경 지원이 되지 않기 때문에 대신 피직스 상태 변경을 사용해 중력 토글
    GeometryComponent->SetCollisionProfileName("TRBreakableDecoration");
    GeometryComponent->SetEnableDamageFromCollision(false);
    GeometryComponent->SetGenerateOverlapEvents(false);
    GeometryComponent->bApplyImpulseOnDamage = false;
    GeometryComponent->SetCanEverAffectNavigation(false);

    GeometryComponent->SetNotifyRemovals(true);

    // 메시 컴포넌트 해제
    if (MeshComponent)
    {
        MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        MeshComponent->SetSimulatePhysics(false);
        MeshComponent->Deactivate();
        MeshComponent->SetGenerateOverlapEvents(false);
        MeshComponent->SetHiddenInGame(true);
        MeshComponent->SetCanEverAffectNavigation(false);
    }
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

    if (GeometryComponent)
    {
        if (HasAuthority())
        {
            CollisionComponent->OnComponentHit.AddDynamic(this, &ABreakableActor::Server_OnHit);
            CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABreakableActor::Server_OnOverlapBegin);
            GeometryComponent->OnChaosRemovalEvent.AddDynamic(this, &ABreakableActor::Server_OnRemoval);
        }
    }
}

void ABreakableActor::Local_ProcessCrumbling(FVector Velocity)
{
    if (!GeometryComponent) return;
    GeometryComponent->SetSimulatePhysics(true);
    GeometryComponent->CrumbleActiveClusters(); // 수동으로 파편화

    // 대략적인 속도 방향으로 파편 산개
    float ExplosionPower = FMath::Clamp(Velocity.Size(), MinCrumbleSpeed, MaxCrumbleSpeed);
    if (ExplosionPower > 0.0f)
    {
        int32 NumPieces = GeometryComponent->GetNumElements(FGeometryCollection::TransformGroup);
        for (int32 i = 0; i < NumPieces; ++i)
        {
            FVector ExplDirection = (FMath::VRand() + FVector::UpVector + Velocity.GetSafeNormal() * 2.5/*가중치 더 세게 줌*/).GetSafeNormal();
            GeometryComponent->ApplyLinearVelocity(i, ExplDirection * ExplosionPower);
        }
    }

    if (!CollisionComponent) return;
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionComponent->Deactivate();
}

void ABreakableActor::Server_OnRemoval(const FChaosRemovalEvent& RemovalEvent)
{
    if (GeometryComponent)
    {
        if (GeometryComponent->OnChaosRemovalEvent.IsBound())
        {
            GeometryComponent->OnChaosRemovalEvent.Clear();
        }
    }
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABreakableActor::DestroySelf);
}

void ABreakableActor::DestroySelf()
{
    Destroy();
}

void ABreakableActor::Server_OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!HasAuthority()) return;
    Server_OnCollision(OtherActor);
}

void ABreakableActor::Server_OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;
    Server_OnCollision(OtherActor);
}

void ABreakableActor::Server_OnCollision(AActor* OtherActor)
{
    if (!HasAuthority()) return;
    if (CanBreak(OtherActor))
    {
        Server_ProcessCrumbling(OtherActor->GetVelocity());
    }
}

void ABreakableActor::Server_ProcessCrumbling(const FVector& CrumblingVelocity)
{
    if (!HasAuthority()) return;
    Multicast_ProcessCrumbling(CrumblingVelocity);
}

bool ABreakableActor::CanBreak(AActor* CollidedActor)
{
    if (CollidedActor)
    {
        return CollidedActor->GetVelocity().Size() >= UE_SMALL_NUMBER;
    }
    return false;
}

void ABreakableActor::Multicast_ProcessCrumbling_Implementation(FVector CrumblingVelocity)
{
    Local_ProcessCrumbling(CrumblingVelocity);
}

