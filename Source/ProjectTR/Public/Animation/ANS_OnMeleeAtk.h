// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_OnMeleeAtk.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTTR_API UANS_OnMeleeAtk : public UAnimNotifyState
{
	GENERATED_BODY()

	UANS_OnMeleeAtk();
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	void TrackMeleeTargets(USkeletalMeshComponent* MeshComp);

protected:
	// 근접 공격 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRightHandMelee = true;

	// NOTE: ANS는 인스턴스가 에셋 단위로 공유될 수 있기 때문에, Stateless하게 유지하는 것이 강력하게 권장됨
};
