// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Engine/ActorChannel.h"

#include "Core/TREnums.h"
#include "DataAssets/TierFxConfig.h"
#include "BaseItem.generated.h"

USTRUCT(BlueprintType)
struct FItemAttribute
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AttrName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AttrValue = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemAttrType AttrType = EItemAttrType::IAT_NEUTRAL_NORMAL;

public:
	FItemAttribute() {}
	FItemAttribute(FString Name, FString Value, EItemAttrType Type)
	{
		AttrName = Name;
		AttrValue = Value;
		AttrType = Type;
	}

	// 타입순 정렬
	bool operator<(const FItemAttribute& Other) const
	{
		return AttrType < Other.AttrType;
	}
};

UCLASS()
class PROJECTTR_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// PostInitializeComponents 내에서 더 세부적으로 처리 시점을 구분한다
	// 이 함수들은 서버와 클라 모두에서 호출되므로, 내부적으로 net role을 구분해서 분기를 나눠야 한다
	virtual void OnPostInitializeComponents();
	virtual void OnPostInvObjectGeneration();

public:
	virtual void Tick(float DeltaTime) override;

	// 아이템 소유자는 아이템에 바인딩된 인벤토리 오브젝트의 소유자로 정의한다
	class AGameCharacter* GetItemOwner() const;

	// 장착 이전 처리 로직
	virtual void Local_PrepareAttachment() { /* 필요 시 구현 */ }

#pragma region /** Networking */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(ABaseItem, InvObject);
		DOREPLIFETIME(ABaseItem, MeshComponent);
		DOREPLIFETIME(ABaseItem, TierVFXReference);
	}
#pragma endregion

#pragma region /** Components */
public:
	// 아이템 리치 판정용 컴포넌트
	// NOTE: 아이템 타입에 따라 어태치된 부모 컴포넌트가 다를 수 있다
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<class UBoxComponent> ReachComponent = nullptr;

	// 아이템 메쉬 (콜리전, 피직스)
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UMeshComponent> MeshComponent = nullptr;

	/* Getters */
	// 메쉬
	UMeshComponent* GetMeshComponent() const { return MeshComponent; }

	// 피직스 연산 시 사용할 컴포넌트
	UPrimitiveComponent* GetPhysComponent() const { return Cast<UPrimitiveComponent>(GetRootComponent()); }

protected:
	// 루트를 메쉬 컴포넌트로 변경한다
	void SetRootToMeshComponent();
#pragma endregion

#pragma region /** Initializer */
protected:
	// 루트 컴포넌트 초기화
	// 루트가 메쉬 컴포넌트이던, 박스 컴포넌트이던 무엇이던간에
	// 루트에 대해 반드시 실행해야 하는 로직을 정의한다
	void InitRootComp(UPrimitiveComponent* Component);

	// 메쉬의 값들을 초기화한다
	// BaseItem은 기본적으로 메쉬를 루트로 사용하지만,
	// 만약 메쉬가 루트가 아닌 경우 이 함수를 오버라이드 해 재설정해주어야 한다
	virtual void InitMeshComp(UPrimitiveComponent* Component);

	// 아이템 리치 판정 컴포넌트 초기화
	virtual void InitReachComp(UPrimitiveComponent* Component);
#pragma endregion

#pragma region /** Collision */
protected:
	// 리치 콜리전 시 로직
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// 아이템이 기본적으로 중력을 연산해야 하는지 여부. 현재 상태와 다를 수 있음
	// e.g. 무기 장착을 할 경우 중력 연산이 일시적으로 중단됨
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	int DefaultItemCollisionWithPawn = ECollisionResponse::ECR_Ignore;

public:
	// 모든 피직스 콜리전을 해제한다
	void DisableItemCollision();

	// 함수를 호출한 시점에서 이 아이템이 플레이어 폰 및 봇 폰과 블로킹 하는지 여부
	bool DoesItemBlockWithPawn() const;

	// 이 콜리전 및 오버랩 설정을 기본 설정으로 되돌린다
	void SetItemCollisionWithPawnToDefault();

	// 이 아이템의 콜리전 및 오버랩 설정을 주어진 값으로 설정한다
	void SetItemCollisionWithPawnTo(ECollisionResponse ColResponse);

protected:
	// 리치 컴포넌트의 크기에 아이템 메시에 대해 얼마만큼의 여백을 줄 것인지 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ReachCompMargin = 30.0f;
#pragma endregion

#pragma region /** Physics */
/* Gravity */
protected:
	// 아이템이 기본적으로 중력을 연산해야 하는지 여부. 현재 상태와 다를 수 있음
	// e.g. 무기 장착을 할 경우 중력 연산이 일시적으로 중단됨
	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	bool bShouldItemSimulateGravity = true;

public:
	// 함수를 호출한 시점에서 이 아이템이 중력을 연산하고 있는지 여부
	bool IsItemSimulatingGravity() const;

	// 이 아이템의 중력 설정을 기본 중력 설정으로 되돌린다
	void SetItemGravityBackToDefault();

	// 이 아이템의 중력 설정을 주어진 값으로 설정한다
	void SetItemGravityTo(bool bGravity);

/* Physics */
protected:
	// 아이템이 기본적으로 물리 연산을 하는지 여부. 현재 상태와 다를 수 있음
	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	bool bShouldItemSimulatePhysics = true;

public:
	// 함수를 호출한 시점에서 이 아이템이 물리 연산을 하고 있는지 여부
	bool IsItemSimulatingPhysics() const;

	// 이 아이템의 물리 연산 설정을 기본 설정으로 되돌린다
	void SetItemPhysicsBackToDefault();

	// 이 아이템의 물리 연산 설정을 주어진 값으로 설정한다
	void SetItemPhysicsTo(bool bPhysics);
#pragma endregion

#pragma region /** Item Data */
protected:
	// 아이템 데이터 클래스
	UPROPERTY(EditAnywhere, Category = "Item Data")
	TSubclassOf<class UItemData> ItemDataClass = nullptr;

protected:
	// 이 아이템에 대응되는 아이템 데이터 객체를 생성해 반환한다
	class UItemData* GenerateItemData(UObject* Outer);

public:
	// 인자로 전달된 InvObject에 할당된 정보를 기반으로 이 아이템의 정보를 복구한다
	// 인자로 전달된 InvObject는 이 액터에 바인딩된 InvObject와는 무관하므로, 이 함수는 아이템 복구 및 복사에 모두 사용될 수 있다
	// (대부분의 경우 이 액터는 아직 InvObject가 바인딩조차 되어있지 않을 수 있다)
	// 성공 여부를 반환한다
	
	// 패스를 나누어 분할해 처리하며, 모두 처리되어야 비로소 완전한 복구가 완료된다
	virtual bool Server_RestoreItem_PreSpawn(const class UInvObject* SrcInvObject);
	virtual bool Server_RestoreItem_PostSpawn(const class UInvObject* SrcInvObject);

	// 이 아이템의 현재 정보를 ItemData에 캐싱한다
	void CacheToItemData() const;
#pragma endregion

#pragma region /** Interface */
protected:
	// 아이템 설명을 생성한다
	// 정적으로 바인딩 되어있을 경우 그대로 그 값을 사용한다
	virtual void GenerateInvObjDescription();

public:
	// 최초 1회 아이콘을 생성한다
	// 이 함수는 새 스테이지 액터를 생성하기 때문에 일반적으로 처음 1회에만 사용되지만,
	// 레벨 트랜지션과 같이 스테이지 액터가 유효하지 않은 경우 사용될 수 있다
	// 이미 스테이지 액터가 존재하는 상황에서의 아이콘 갱신은 Host_ProcessRefreshIcon을 사용해야 한다
	UFUNCTION()
	void Server_InitializeIcon();

public:
	// 이 변수들은 인벤토리 오브젝트에 전달할 값을 임시로 담거나, 혹은 BP에 노출시켜 수동으로 입력하기 위한 임시 값으로
	// 실제 게임플레이 상에서 아이템 설명을 가져오기 위해서는 InvObject를 사용해야 한다
	UPROPERTY(EditDefaultsOnly)
	FString TempItemDesc = "";

	UPROPERTY(EditDefaultsOnly)
	TArray<FItemAttribute> TempItemAttributesForUI;

	// 아이콘 생성 시 아이템을 얼마나 기울인 상태에서 생성할 것인지 지정
	UPROPERTY(EditAnywhere)
	FRotator IconDisplayRotation = FRotator();

	// 최초 생성 시 아이콘을 초기해야하는지 여부
	UPROPERTY(EditDefaultsOnly)
	bool bShouldInitializeIcon = true;

	// 서버에서 아이콘 생성(갱신) 요청이 1회 이상 처리되었는지 여부
	// 서버에서 처리되었다면 클라도 처리된 것으로 간주
	bool bServer_HasInitializedIcon = false;
#pragma endregion

#pragma region /** Inventory */
protected:
	// InvObject 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<class UInvObject> InvObjectClass = nullptr;

	// InvObject
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Inventory")
	TObjectPtr<class UInvObject> InvObject = nullptr;

public:
	// 이 아이템에 대응되는 InvObject와 ItemData를 생성 및 등록한다
	void GenerateInvObject(UObject* Outer);

	// 아이템 픽업 시 호출; 성공적으로 인벤토리에 추가되었는지 여부 반환
	bool OnItemPickup(class UInventoryComponent* InvComp);

	// InvObject Setter
	void SetInvObject(class UInvObject* InvObj);

	// InvObject Getter
	TObjectPtr<class UInvObject> GetInvObject();

protected:
	// InvObject가 등록되있는지 확인한다
	void VerifyInvObject();

	// 기본값 InvObject를 생성한다
	UInvObject* CreateDefaultInvObject();
#pragma endregion

#pragma region /** Size */
protected:
	// 이 아이템의 물리적 크기 및 크기 extent의 액터 중심으로부터의 오프셋을 계산해 멤버 변수 값을 업데이트한다
	// 다만 이 값이 변경된다고 해서 연관된 컴포넌트들의 크기나 오프셋은 변하지 않으므로 직접 수정해주어야 한다
	// 아이템 타입에 따라 실행 시점이 다를 수 있다
	virtual void RefreshItemSizeAndOffset();

	// 함수 호출 전에 RefreshItemSizeAndOffset가 호출되어야 유효한 값이 적용된다
	// 아이템 타입에 따라 실행 시점이 다를 수 있다
	virtual void AdjustComponentsToMatchItemSize();

public:
	FVector GetItemSize() const { return ItemSize; }
	FVector GetItemSizeBoxOffset() const { return ItemSizeBoxOffset; }

protected:
	/* 중요 */
	// ItemSize, ItemSizeBoxOffset의 올바른 값을 구하기 위해서 먼저 대상 액터를 월드로테이션에 정렬을 해야 한다
	// 이는 바운딩 박스가 월드로테이션(=(0,0,0))에서만 실제 범위와 동일하기 때문임
	// 따라서 값을 구한 이후 이 값들을 다른 무언가에 사용할 때에도 적용할 대상을 먼저 월드로테이션으로 만든 후에 적용해야 한다
	FVector ItemSize;

	// 이 값은 동적으로 크기가 결정되는 아이템에 한해서만 0이 아닌 값을 갖는다
	// 이 값이 어떤 역할을 하는지 이해하는 게 중요한데,
	// GunItem을 예시로 들면, Gun은 모든 컴포넌트들의 박스들을 고려해 하나의 큰 콜리전 박스를 동적으로 생성한다
	// 이때 생성된 콜리전 박스는 크기는 정확하지만 위치가 총기 아이템과 서로 맞지 않는데,
	// 이는 총기 파츠 메시들이 중앙 정렬되어있지 않기 때문이다
	// 따라서 이 오프셋 값을 사용해 컴포넌트들간의 위치 관계를 조정하는 것으로 총기와 콜리전 박스를 정렬한다
	FVector ItemSizeBoxOffset = FVector::ZeroVector;
#pragma endregion

#pragma region /** Attachment */
/* Attachment */
protected:
	// 장착자 메쉬에 탈부착할 때의 룰 지정
	FAttachmentTransformRules EquipAttachRule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	FDetachmentTransformRules EquipDetachRule = FDetachmentTransformRules::KeepWorldTransform; // Default

	// 캐릭터 메쉬에 부착할 때의 이 아이템의 상대적 값들 지정
	// 이 값들은 BP에서 직접 세부 조정을 해줘야 함
	// 위치
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attach")
	FVector AttachRelativeLocation;

	// 회전
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attach")
	FRotator AttachRelativeRotation;

	// 캐릭터 메쉬에 부착할 때 캐릭터의 어느 부위에 부착할지 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attach")
	ECharacterParts AttachPart = ECharacterParts::ECP_PrimaryWield;

	// 현재 이 아이템이 부착되어있는 대상의 소켓의 이름
	FName AttachedSocketName;

public:
	// Getters
	const FAttachmentTransformRules GetEquipAttachRule() { return EquipAttachRule; }
	const FDetachmentTransformRules GetEquipDetachRule() { return EquipDetachRule; }
	const FVector GetAttachRelativeLocation() { return AttachRelativeLocation; }
	const FRotator GetAttachRelativeRotation() { return AttachRelativeRotation; }
	const ECharacterParts GetCharacterAttachPartName() { return AttachPart; }
#pragma endregion

#pragma region /** Visuals */
public:
	// 이 아이템의 렌더링 여부를 변경하는 걸 요청한다
	// 동적으로 생성된 메쉬가 있을 경우, 모든 메쉬들의 레플리케이션이 처리 완료된 시점에 처리해야 한다
	virtual void RegisterVisibility(bool bVisibility);

	// 이 아이템의 렌더링 여부를 설정한다
	// 오버라이드 시 부모 함수를 콜해야 한다
	virtual void SetItemVisibility(bool bVisibility);

	// 이 아이템의 등급 별 특수효과를 나타내는 나이아가라 컴포넌트를 생성한 후 부착한다
	class UNiagaraComponent* Local_SpawnAndAttachTierEffect();

protected:
	UFUNCTION()
	void OnRep_TierVFXReference();

public:
	UPROPERTY(EditDefaultsOnly)
	class UTierFxConfig* TierFxConfig = nullptr;

	// 현재 사용중인 티어 VFX
	// NOTE: 현재는 런타임 VFX 변경은 지원하지 않음
	// 정확히 말하면 NULL에서 NOT NULL로 바뀌어 런타임에 1회 초기화하는 것은 가능하지만
	// NOT NULL -> NULL or NOT NULL은 불가능함
	// 다만 추후 필요 시 기존 나이아가라 컴포넌트 파괴 후 생성하는 것으로 쉽게 구현 가능
	UPROPERTY(ReplicatedUsing = OnRep_TierVFXReference)
	ETierNiagaraReference TierVFXReference = ETierNiagaraReference::ENR_NULL;

	// 이 아이템의 Tier vfx 사용 여부
	// 이 값은 PostInitializeComponents 이전에 설정되어 있어야 한다
	// 이 값이 false면 TierVFXReference도 NULL 상태를 유지하기 때문에 자연스럽게 클라이언트에도 상태가 동기화된다
	UPROPERTY(EditAnywhere)
	bool bServer_UseTierVFX = true;

protected:
	// RegisterVisibility 호출 이후 아직 SetItemVisibility가 호출되지 않았을 경우 true로 설정한다
	bool bShouldRenewItemVisibility = false;

	// 이 아이템의 가시성 여부
	// NOTE: 이 값은 클라이언트의 동적 메쉬에서는 즉각적으로 적용되지 않을 수 있다
	// 이 경우 동적메쉬들의 레플리케이션이 끝나는 대로 값이 적용된다
	bool bItemVisibility = true;
#pragma endregion
};
