// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Core/ReplicatedObject.h"
#include "ItemData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTTR_API UItemData : public UReplicatedObject
{
	GENERATED_BODY()

public:
	UItemData();

	// 아이템의 정보를 이 데이터에 캐싱한다
	// 성공 여부를 반환한다
	virtual bool CacheItem(const class ABaseItem* Item);

	// 재귀적으로 Outer를 변경한다
	virtual void ChangeOuterRecursive(UObject* NewOuter, bool bAddToRootSet);

	// 아이콘 초기화 여부를 캐싱한다
	// 1회 이상 동적 생성했을 경우 true
	void CacheHasInitIcon(bool bValue) { bCachedHasInitIcon = bValue; }
	bool GetCachedHasInitIcon() { return bCachedHasInitIcon; }

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bCachedHasInitIcon = false;

#pragma region /** Debug */
public:
	UPROPERTY(EditAnywhere)
	float TestVal = 0.0f;
#pragma endregion
};
