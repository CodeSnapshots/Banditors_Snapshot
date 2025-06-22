// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Map.h"
#include "Net/UnrealNetwork.h"

#include "Core/ReplicatedObject.h"
#include "Core/TREnums.h"
#include "StatusEffect/StatusEffect.h"
#include "InvObject.generated.h"

USTRUCT(BlueprintType)
struct FInvObjSize
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 X = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Y = 0;
};

/**
 * InvObject는 ABaseItem의 생성에 의해서 부가적으로 생성하는 것을 권장하며, UInvObject 객체를 직접 생성하는 행위는 지양한다.
 * 액터가 필요하지 않은 경우에도 (e.g. 창고 안에 들어있는 상태의 아이템을 생성) 우선 액터를 생성하고 그 후 액터로부터 Detach하는 형태로 처리한다.
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTTR_API UInvObject : public UReplicatedObject
{
	GENERATED_BODY()

public:
	UInvObject();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(UInvObject, ItemData);
		DOREPLIFETIME(UInvObject, ItemName);
		DOREPLIFETIME(UInvObject, ItemDesc);
		DOREPLIFETIME(UInvObject, ItemAttributesForUI);
		DOREPLIFETIME(UInvObject, CurrIconStageActor);
		DOREPLIFETIME(UInvObject, Tier);
	}

public:
	// 이 InvObject에 대응되는 아이템을 생성하고 생성된 아이템에 ItemData를 등록한다
	class ABaseItem* GenerateAndSpawnItem(UObject* Outer, FVector Location, FRotator Rotation, FActorSpawnParameters Params, bool bRestoreUsingItemData, bool bUseTierVFX);

	// 이 오브젝트 및 이 오브젝트 하위의 모든 오브젝트들에 대해 수동으로 아우터를 변경한다
	// Owner가 아닌 Outer임에 유의
	// bAddToRootSet가 true면 루트셋에 추가한다
	virtual void ChangeOuterRecursive(UObject* NewOuter, bool bAddToRootSet);

	// InvObject의 Owner는 해당 오브젝트를 인벤토리 컴포넌트 내에 보유하고 있는 캐릭터로 정의한다
	class AGameCharacter* GetInvObjectOwner() const;

#pragma region /** Gameplay */
/* Status Effects */
// 아이템 스테이터스 이펙트 인스턴스는 구조체 값을 기반으로 생성된다
// 이 구조체 값은 정적으로 에디터 상에서 값을 지정할 수도 있고, 필요 시 동적으로 결정되도록 만들 수 있다
public:
	// 보유자(InventoryComp 혹은 그 상속 컴포넌트에 추가된 경우 컴포넌트의 오너)에게 부여할 스테이터스 이펙트 데이터
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	FStatEffectGenInfo OwnerStatusEffectData;

	// 사용자(Deploy한 오너)에게 부여할 스테이터스 이펙트 데이터
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	FStatEffectGenInfo WielderStatusEffectData;

	// 보유자에게 지정된 스테이터스 이펙트를 부여할지 여부
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	bool bApplyStatusEffectToOwner = false;

	// 사용자에게 지정된 스테이터스 이펙트를 부여할지 여부
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	bool bApplyStatusEffectToWielder = false;

	// 이 오브젝트가 어떤 캐릭터에게 스테이터스 이펙트를 부여중인 경우 해당 인스턴스를 캐싱
	TWeakObjectPtr<class UStatusEffect> CachedOwnerStatEffect = nullptr;
	TWeakObjectPtr<class UStatusEffect> CachedWielderStatEffect = nullptr;

/* Shop */
protected:
	// NOTE:
	// 가격은 한번 결정된 이후 변해서는 안된다
	// 총기를 비롯한 일부 아이템의 경우 가격이 런타임에 결정될 수 있다
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	int32 Price = 1;

	// 판매 가능 여부
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	bool bMarketable = true;

public:
	// Getters
	const int32 Local_GetPrice() const { return Price; }
	const bool Local_IsMarketable() const { return bMarketable; }

/* Tier */
private:
	// NOTE:
	// 티어는 한번 결정된 이후 변해서는 안된다
	// 총기를 비롯한 일부 아이템의 경우 티어가 런타임에 결정될 수 있다, 이 경우 반드시 setter를 사용해야 한다
	// NOTE:
	// Unspecified와 None을 구분지어야 한다
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_Tier, Category = "Gameplay")
	EItemTier Tier = EItemTier::IT_TIER_UNSPECIFIED;

protected:
	UFUNCTION()
	void OnRep_Tier();

public:
	const EItemTier Host_GetTier() const { return Tier; }

	// 티어를 수정하는 것은 티어 결정 시점이 런타임인 특수한 경우(e.g. 총기)에 한해 허용된다
	void Server_ChooseTierDuringRuntime(EItemTier NewTier);
#pragma endregion

#pragma region /** Item Data */
protected:
	// 아이템 데이터
	// 아이템 액터 생성 시 초기화
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Item Data")
	TObjectPtr<class UItemData> ItemData = nullptr;

	// 대응되는 아이템 클래스
	// 아이템 액터 생성 시 초기화
	UPROPERTY()
	TSubclassOf<class ABaseItem> BaseItemClass = nullptr;

public:
	/* Setters */
	void SetItemData(class UItemData* Data);
	void SetBaseItemClass(TSubclassOf<class ABaseItem> Class);

	/* Getters */
	class UItemData* GetItemData() const { return ItemData; }
	TSubclassOf<class ABaseItem> GetBaseItemClass() { return BaseItemClass; }
#pragma endregion

#pragma region /** Logic */
/* Inventory grid */
protected:
	// 인벤토리 격자에서 차지하는 X 크기 (고정)
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 InvXSize = 1;

	// 인벤토리 격자에서 차지하는 Y 크기 (고정)
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 InvYSize = 1;

public:
	// 이 InvObject의 격자 크기를 X, Y 순으로 반환한다
	UFUNCTION(BlueprintCallable)
	FInvObjSize GetDimensions() const;
#pragma endregion

#pragma region /** Interface */
/* Description */
// 이 값들은 오로지 UI 표기를 위해서만 사용된다
protected:
	UPROPERTY(Replicated, EditDefaultsOnly)
	FString ItemName = "";

	UPROPERTY(Replicated)
	FString ItemDesc = "";

	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<FItemAttribute> ItemAttributesForUI;

public:
	UFUNCTION(BlueprintCallable)
	const FString GetInvObjDesc() const { return ItemDesc; }
	void SetInvObjDesc(FString Desc) { ItemDesc = Desc; }

	UFUNCTION(BlueprintCallable)
	const FString GetInvObjName() const { return ItemName; }
	void SetInvObjName(FString Name) { ItemName = Name; }

	UFUNCTION(BlueprintCallable)
	const TArray<FItemAttribute> GetInvObjAttr() const { return ItemAttributesForUI; }
	void SetInvObjAttr(const TArray<FItemAttribute>& Attr) { ItemAttributesForUI = Attr; }

/* Icon */
protected:
	// 다이나믹 매터리얼 생성에 사용할 베이스 매터리얼
	TObjectPtr<UMaterial> BaseIconMaterial = nullptr;

	// 아이콘 매터리얼
	UPROPERTY(EditDefaultsOnly, Category = "Icon")
	TObjectPtr<UMaterialInstance> IconMat = nullptr;
	
	// 아이콘 매터리얼이 Dynamic한지 여부; 렌더타깃 등의 기능을 사용하기 위해서는 Dynamic material이 필요하다
	// 이 값은 캐싱을 통해 보존되지 않는다
	bool bLocal_IsIconMatDynamic = false;

public:
	// 현재 아이콘 매터리얼 Getter
	UFUNCTION(BlueprintCallable)
	UMaterialInstance* GetCurrIcon() const;

	// 아이콘 매터리얼 Getter
	FORCEINLINE UMaterialInstance* GetIcon() const;

/* IconStage */
public:
	UPROPERTY(ReplicatedUsing = OnRep_IconStageActor)
	TWeakObjectPtr<class AIconStageActor> CurrIconStageActor = nullptr;

protected:
	UFUNCTION()
	void OnRep_IconStageActor();

public:
	// 클라이언트 아이콘 생성 로직의 시작점이자, 실질적 로직 구현부
	void Local_InitIconStageActor();
#pragma endregion
};
