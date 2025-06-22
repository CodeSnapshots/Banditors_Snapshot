// Copyright (C) 2024-2025 by Haguk Kim


#include "Items/BaseItem.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "Core/TRMacros.h"
#include "Core/CustomUtil.h"
#include "Core/TRUtils.h"
#include "Core/TRStructs.h"
#include "Core/TRCVar.h"
#include "Core/ProjectTRGameModeBase.h"
#include "DataAssets/TierFxConfig.h"
#include "Items/ItemData.h"
#include "Items/WieldItem.h"
#include "Inventory/InvObject.h"
#include "Inventory/InventoryComponent.h"
#include "Characters/FPSCharacter.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region /** Networking */
    bReplicates = true;
    SetReplicateMovement(true);
#pragma endregion

#pragma region /** Component Initialization */
    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    }

    // MeshComponent
    if (!MeshComponent)
    {
        MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
        InitMeshComp(MeshComponent);
        MeshComponent->SetupAttachment(RootComponent);
    }

    // ReachComponent
    if (!ReachComponent)
    {
        ReachComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ReachComponent"));
        InitReachComp(ReachComponent);
        ReachComponent->SetupAttachment(MeshComponent);
    }
#pragma endregion

    // 기본 VFX 컨픽 바인딩
    if (!TierFxConfig)
    {
        static ConstructorHelpers::FObjectFinder<UTierFxConfig> TierFXFinder(TEXT(ASSET_DEFAULT_TIER_FX));
        if (TierFXFinder.Succeeded())
        {
            TierFxConfig = TierFXFinder.Object;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ABaseItem - Unable to find default Tier FX config asset!"));
        }
    }
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

    // 최초 1회에 한해 아이콘 초기화; 이후 변경사항 발생 시 직접 Refresh 해주어야함
    // Server_InitializeIcon는 매 BeginPlay마다 호출되지만 내부적으로 최초 1회에 한해 로직 실행
    if (HasAuthority() && bShouldInitializeIcon && !bServer_HasInitializedIcon)
    {
        Server_InitializeIcon();
    }
}

void ABaseItem::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    OnPostInitializeComponents();

    // NOTE: 아래 작업들은 OnPostInitializeComponents 이후에 처리되어야 하는데,
    // OnPostInitializeComponents 과정에서 아래 작업들의 결과에 영향을 줄 만한 값이 변경될 소지가 있기 때문이다.

    // 인벤토리 오브젝트 없을 경우 최초 생성
    if (!IsValid(InvObject))
    {
        GenerateInvObject(GetWorld() /* 초기 아우터는 월드로 설정; 즉 InvObject는 아이템 액터를 레퍼런스 하지 않음 */);
        GenerateInvObjDescription();
        VerifyInvObject();
    }

    UPrimitiveComponent* PhysComp = GetPhysComponent();
    if (PhysComp)
    {
        PhysComp->bReplicatePhysicsToAutonomousProxy = true;
    }

    OnPostInvObjectGeneration();

    if (HasAuthority() && bServer_UseTierVFX)
    {
        // 서버에서는 이 시점에 아이템의 종류와 무관하게 티어가 결정되어 있어야 함
        TierVFXReference = TRUtils::GetNiagaraRefFromTier(InvObject->Host_GetTier(), true);

        // 서버의 경우 수동 호출
        Local_SpawnAndAttachTierEffect();
    }
}

void ABaseItem::OnPostInitializeComponents()
{
    // NOTE:
    // 실행 시점은 AActor::PostInitializeComponents 직전이 가장 적합하며, 다른 곳에서 처리할 경우
    // 레플리케이션이 제대로 안되어 Translation이 제대로 전달되지 않거나, 혹은 
    // 에셋을 메모리에 올려둔 채 다른 곳에서 에셋에 대한 편집(블루프린트 컴파일 등) 시
    // 엔진이 에셋의 프로퍼티를 Reinitialize하게 되면 엔진단에서 응답없음이 발생할 수 있음.

    // 주의: PostInitializeComponents 단에서 컴포넌트 계층구조의 변화를 유발하는 함수는 서버와 클라 모두에서 실행해주어야 한다
    SetRootToMeshComponent();

    // 메시 컴포넌트 초기화 및 등록 완료되었으므로 액터 전체 크기를 계산하고, 연관된 컴포넌트들을 조정한다
    // 서버와 클라 모두에서 실행해주어야 한다
    RefreshItemSizeAndOffset();
    AdjustComponentsToMatchItemSize();
}

void ABaseItem::OnPostInvObjectGeneration()
{
    // 필요 시 추가
}

void ABaseItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

AGameCharacter* ABaseItem::GetItemOwner() const
{
    if (!InvObject) return nullptr;
    return InvObject->GetInvObjectOwner();
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseItem::SetRootToMeshComponent()
{

    // 루트를 메쉬로 변경
    // NOTE:
    // 블루프린트 에디터에서 루트를 메쉬 컴포넌트로 설정한 C++ 클래스를 편집시 Detail 패널에 정보가 표시되지 않아 콘텐츠 제작이 어려움.
    // 따라서 생성자에서는 임의의 Scene Component를 만들어 그 아래에 메쉬 컴포넌트를 붙이고,
    // 게임 로직이 실행되기 전에 루트를 메쉬 컴포넌트로 변경해주는 것으로 루트 기반 트랜스폼 처리가 가능하면서 동시에 에디터 편집이 가능하게끔 구조를 제작함.
    // 단 이때 루트 컴포넌트의 수정은 레플리케이션 이전에 처리해주어야 하는데,
    // 그래야 AttachmentReplication을 통해 올바른 트랜스폼 오프셋과 Relative scale이 전달되어 클라이언트가 제대로 된 정보를 사용할 수 있기 때문임.

    // 이 부분을 처리해줘야 자기 자신에게 Attach하려는 상황을 막을 수 있음 
    // AttachToComponent() 내의 경고 발생 지점에 브레이크포인트 걸고 콜백 타고 가면 확인 가능함
    if (IsValid(MeshComponent->GetAttachParent()))
    {
        MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    }

    // 이걸 해주지 않았을 경우 컴포넌트를 교체해주는 것만으로 Transform 정보가 적용이 되지 않음
    // 이때 Scale 정보는 이미 올바른 값이 적용된 상태이므로 변경하지 않음
    FVector RootLocation = RootComponent->GetComponentLocation();
    FRotator RootRotation = RootComponent->GetComponentRotation();

    RootComponent = nullptr;
    if (!SetRootComponent(MeshComponent))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to change the ABaseItem root component to mesh / Auth : %d / Outer: %s"), HasAuthority(), *(MeshComponent->GetOuter()->GetName()));
        return;
    }
    MeshComponent->SetWorldLocation(RootLocation, false, nullptr, ETeleportType::ResetPhysics);
    MeshComponent->SetWorldRotation(RootRotation, false, nullptr, ETeleportType::ResetPhysics);
}

void ABaseItem::InitRootComp(UPrimitiveComponent* Component)
{
    check(Component != nullptr);

    Component->SetIsReplicated(true);
}

void ABaseItem::InitMeshComp(UPrimitiveComponent* Component)
{
    check(Component != nullptr);

    InitRootComp(Component);

    Component->SetComponentTickEnabled(false);
    Component->SetSimulatePhysics(bShouldItemSimulatePhysics);
    Component->SetEnableGravity(bShouldItemSimulateGravity);

    Component->SetCollisionProfileName(TEXT("BaseItemMesh"));
    Component->SetGenerateOverlapEvents(false); // 불필요; 단 소울 아이템과 같이 오버랩이 필요한 경우 함수를 상속받아 따로 설정해주어야 함
    Component->SetCollisionResponseToChannel(ECC_PlayerPawn, ECollisionResponse(DefaultItemCollisionWithPawn));
    Component->SetCollisionResponseToChannel(ECC_BotPawn, ECollisionResponse(DefaultItemCollisionWithPawn));
    Component->SetCanEverAffectNavigation(false);

    TRUtils::OptimizePrimitiveComp(Component, false, false/*라이팅 허용*/);
}

void ABaseItem::InitReachComp(UPrimitiveComponent* Component)
{
    check(Component != nullptr);

    Component->SetComponentTickEnabled(false);
    Component->SetIsReplicated(true);
    Component->SetSimulatePhysics(false);
    Component->SetCollisionProfileName(TEXT("ItemReachComp"));
    Component->SetGenerateOverlapEvents(false); // 불필요; 히트를 사용해 인터랙트함
    Component->SetShouldUpdatePhysicsVolume(false);
    Component->SetCanEverAffectNavigation(false);

    // 충돌 시 호출 함수 맵핑
    if (GetLocalRole() == ROLE_Authority)
    {
        Component->OnComponentHit.AddDynamic(this, &ABaseItem::OnHit);
    }
}

void ABaseItem::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    // 필요 시 추가
}

void ABaseItem::DisableItemCollision()
{
    if (UPrimitiveComponent* ItemPhysComp = GetPhysComponent())
    {
        ItemPhysComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        // 필요 시 SetCollisionResponseTo 함수로 Fine tuning 가능
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("Item %s has no root component, or the root component is not a derivative of UPrimitiveComponent."), *GetName());
    return;
}

bool ABaseItem::DoesItemBlockWithPawn() const
{
    if (const UPrimitiveComponent* ItemPhysComp = GetPhysComponent())
    {
        return (ItemPhysComp->GetCollisionResponseToChannel(ECC_PlayerPawn) == ECollisionResponse::ECR_Block) && (ItemPhysComp->GetCollisionResponseToChannel(ECC_BotPawn) == ECollisionResponse::ECR_Block);
    }
    UE_LOG(LogTemp, Error, TEXT("Item %s has no root component, or the root component is not a derivative of UPrimitiveComponent."), *GetName());
    return false;
}

void ABaseItem::SetItemCollisionWithPawnToDefault()
{
    return SetItemCollisionWithPawnTo(ECollisionResponse(DefaultItemCollisionWithPawn));
}

void ABaseItem::SetItemCollisionWithPawnTo(ECollisionResponse ColResponse)
{
    if (UPrimitiveComponent* ItemPhysComp = GetPhysComponent())
    {
        ItemPhysComp->SetCollisionResponseToChannel(ECC_PlayerPawn, ColResponse);
        ItemPhysComp->SetCollisionResponseToChannel(ECC_BotPawn, ColResponse);
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("Item %s has no root component, or the root component is not a derivative of UPrimitiveComponent."), *GetName());
    return;
}

void ABaseItem::AdjustComponentsToMatchItemSize()
{
    // 월드스페이스 정렬
    FRotator OriginRotation = GetActorRotation();
    SetActorRotation(FRotator::ZeroRotator);

    FVector Extent = ItemSize;
    Extent.X += ReachCompMargin;
    Extent.Y += ReachCompMargin;
    Extent.Z += ReachCompMargin;
    ReachComponent->SetBoxExtent(Extent, true);

    // 메시의 실제 시각적인 중간 지점을 구한 후 리치 컴포넌트를 해당 위치로 이동시킨다
    FBoxSphereBounds Bounds = MeshComponent->CalcBounds(MeshComponent->GetComponentTransform());
    FVector RealCenter = Bounds.Origin;
    ReachComponent->SetWorldLocation(RealCenter);

#if WITH_EDITOR
    if (CVarShowDebugShapes.GetValueOnGameThread())
    {
        DrawDebugBox(GetWorld(), ReachComponent->GetComponentLocation(), ReachComponent->GetScaledBoxExtent(), FColor::Red, false, 5.0f, 0, 1.0);
    }
#endif

    // 다시 원래 회전으로 복구
    SetActorRotation(OriginRotation);
}

bool ABaseItem::IsItemSimulatingGravity() const
{
    if (const UPrimitiveComponent* ItemPhysComp = GetPhysComponent())
    {
        return ItemPhysComp->IsGravityEnabled();
    }
    UE_LOG(LogTemp, Error, TEXT("Item %s has no root component, or the root component is not a derivative of UPrimitiveComponent."), *GetName());
    return false;
}

void ABaseItem::SetItemGravityBackToDefault()
{
    return SetItemGravityTo(bShouldItemSimulateGravity);
}

void ABaseItem::SetItemGravityTo(bool bGravity)
{
    if (UPrimitiveComponent* ItemPhysComp = GetPhysComponent())
    {
        return ItemPhysComp->SetEnableGravity(bGravity);
    }
    UE_LOG(LogTemp, Error, TEXT("Item %s has no root component, or the root component is not a derivative of UPrimitiveComponent."), *GetName());
    return;
}

bool ABaseItem::IsItemSimulatingPhysics() const
{
    if (UPrimitiveComponent* ItemPhysComp = GetPhysComponent())
    {
        return ItemPhysComp->IsSimulatingPhysics();
    }
    UE_LOG(LogTemp, Error, TEXT("Item %s has no root component, or the root component is not a derivative of UPrimitiveComponent."), *GetName());
    return false;
}

void ABaseItem::SetItemPhysicsBackToDefault()
{
    return SetItemPhysicsTo(bShouldItemSimulatePhysics);
}

void ABaseItem::SetItemPhysicsTo(bool bPhysics)
{
    if (UPrimitiveComponent* ItemPhysComp = GetPhysComponent())
    {
        return ItemPhysComp->SetSimulatePhysics(bPhysics);
    }
    UE_LOG(LogTemp, Error, TEXT("Item %s has no root component, or the root component is not a derivative of UPrimitiveComponent."), *GetName());
    return;
}

void ABaseItem::CacheToItemData() const
{
    if (!IsValid(InvObject) || !IsValid(InvObject->GetItemData()))
    {
        UE_LOG(LogTemp, Error, TEXT("ItemData is invalid, Failed to cache to item data!"));
        return;
    }
    if (!InvObject->GetItemData()->CacheItem(this))
    {
        UE_LOG(LogTemp, Error, TEXT("ItemData caching has failed!"));
    }
    return;
}

bool ABaseItem::Server_RestoreItem_PreSpawn(const UInvObject* SrcInvObject)
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Error, TEXT("Server_RestoreItem_PreSpawn - Called from client!"));
        return false;
    }
    const UItemData* Data = SrcInvObject->GetItemData();
    if (!IsValid(Data))
    {
        UE_LOG(LogTemp, Error, TEXT("Server_RestoreItem_PreSpawn - Passed-in data is null, Failed to cache to item data!"));
        return false;
    }
    
    this->bServer_HasInitializedIcon = Data->GetCachedHasInitIcon();
    // 필요 시 추가 로직은 개별 클래스에서 구현한다
    return true;
}

bool ABaseItem::Server_RestoreItem_PostSpawn(const UInvObject* SrcInvObject)
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Error, TEXT("Server_RestoreItem_PostSpawn - Called from client!"));
        return false;
    }
    const UItemData* Data = SrcInvObject->GetItemData();
    if (!IsValid(Data))
    {
        UE_LOG(LogTemp, Error, TEXT("Server_RestoreItem_PostSpawn - Passed-in data is null, Failed to cache to item data!"));
        return false;
    }

    // 필요 시 추가 로직은 개별 클래스에서 구현한다
    return true;
}

UItemData* ABaseItem::GenerateItemData(UObject* Outer)
{
    UClass* DataClass = ItemDataClass;
    if (!IsValid(DataClass))
    {
        UE_LOG(LogTemp, Error, TEXT("Item %s has no default ItemDataClass set."), *GetName());
    }
    else
    {
        // 중요: Outer가 이 액터가 아닐 수 있기 때문에, NewObject에 TEXT를 사용해 이름을 전달하면 크래시 발생
        FName UniqueName = MakeUniqueObjectName(Outer, ItemDataClass, FName("ItemData"));
        return NewObject<UItemData>(Outer, ItemDataClass, UniqueName);
    }
    return nullptr;
}

void ABaseItem::GenerateInvObject(UObject* Outer)
{
    UClass* TypeClass = InvObjectClass;
    UInvObject* GeneratedObj = nullptr;

    if (!IsValid(TypeClass))
    {
        UE_LOG(LogTemp, Error, TEXT("Item %s has no default InvObjectClass set."), *GetName());
    }
    else
    {
        // 중요: Outer가 이 액터가 아닐 수 있기 때문에, NewObject에 TEXT를 사용해 이름을 전달하면 크래시 발생
        FName UniqueName = MakeUniqueObjectName(Outer, TypeClass, FName("InvObject"));
        GeneratedObj = NewObject<UInvObject>(Outer, TypeClass, UniqueName);
        GeneratedObj->SetItemData(GenerateItemData(Outer));
        GeneratedObj->SetBaseItemClass(GetClass()); // GetClass는 BP일 경우 BP 클래스를 반환한다
        InvObject = GeneratedObj;
    }
    return;
}

void ABaseItem::VerifyInvObject()
{
    if (!IsValid(InvObject))
    {
        UE_LOG(LogTemp, Warning, TEXT("Item %s has no InvObject Set. Using default object. This could result in an unintended behaviour."), *this->GetName());
        InvObject = CreateDefaultInvObject();
    }
}

UInvObject* ABaseItem::CreateDefaultInvObject()
{
    return NewObject<UInvObject>();
}

void ABaseItem::RefreshItemSizeAndOffset()
{
    if (!GetMeshComponent())
    {
        ItemSize = FVector::OneVector;
        ItemSizeBoxOffset = FVector::ZeroVector;
        return;
    }
    ItemSize = GetMeshComponent()->GetLocalBounds().BoxExtent;
    ItemSizeBoxOffset = FVector::ZeroVector;
};

void ABaseItem::RegisterVisibility(bool bVisibility)
{
    // BaseItem은 동적 메쉬가 없으므로 그냥 바로 처리한다
    SetItemVisibility(bVisibility);
}

void ABaseItem::SetItemVisibility(bool bVisibility)
{
    bShouldRenewItemVisibility = false; // 갱신했으니 false로 처리
    if (GetMeshComponent())
    {
        GetMeshComponent()->SetVisibility(bVisibility, true);
    }
    bItemVisibility = bVisibility;
}

bool ABaseItem::OnItemPickup(UInventoryComponent* InvComp)
{
    if (IsValid(InvComp))
    {
        if (InvComp->TryAddInvObject(this->InvObject))
        {
            if (HasAuthority())
            {
                CacheToItemData();
                Destroy();
            }
            return true;
        }
    }
    return false;
}

void ABaseItem::SetInvObject(UInvObject* InvObj)
{
    InvObject = InvObj;
}

TObjectPtr<class UInvObject> ABaseItem::GetInvObject()
{
    return InvObject;
}

void ABaseItem::GenerateInvObjDescription()
{
    if (!InvObject)
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateInvObjDescription - InvObject is null! Item: %s"), *(GetName()));
        return;
    }
    InvObject->SetInvObjDesc(TempItemDesc);
    InvObject->SetInvObjAttr(TempItemAttributesForUI);
}

void ABaseItem::Server_InitializeIcon()
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Error, TEXT("Server_InitializeIcon - Client should not call this function!"));
        return;
    }
    if (!GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("Server_InitializeIcon - Invalid world!"));
        return;
    }

    UInvObject* InvObj = GetInvObject();
    if (HasAuthority() && InvObj)
    {
        // 캐싱을 처리해야 메쉬 정보가 데이터에 기록되어 아이콘 생성 과정에서 사용할 수 있음
        bServer_HasInitializedIcon = true; // 아이콘 초기화가 완료되었음 또한 캐싱되어야 하므로 CacheToItemData보다 먼저 호출되어야 함
        CacheToItemData(); // 캐싱
        
        AProjectTRGameModeBase* TRGM = Cast<AProjectTRGameModeBase>(GetWorld()->GetAuthGameMode());
        if (TRGM)
        {
            TRGM->UpdateIconOf(InvObj);
        }
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("Server_InitializeIcon - Something went wrong!"));
    return;
}

UNiagaraComponent* ABaseItem::Local_SpawnAndAttachTierEffect()
{
    if (!IsValid(TierFxConfig)) return nullptr;

    UNiagaraComponent* SpawnedComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
        TierFxConfig->SearchNiagaraFromEnum(TierVFXReference),
        GetRootComponent(),
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::KeepRelativeOffset,
        true,
        true
    );

    // 항상 월드 회전 사용
    if (SpawnedComp)
    {
        SpawnedComp->SetUsingAbsoluteRotation(true);
        SpawnedComp->SetUsingAbsoluteScale(true);
    }
    return SpawnedComp;
}

void ABaseItem::OnRep_TierVFXReference()
{
    Local_SpawnAndAttachTierEffect();
}