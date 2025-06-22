// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "IconStageActor.generated.h"

UCLASS()
class PROJECTTR_API AIconStageActor : public AActor
{
	GENERATED_BODY()

public:
#pragma region /** Networking */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(AIconStageActor, DisplayedActor);
		DOREPLIFETIME(AIconStageActor, ReferencingInvObj);
	}
#pragma endregion
	
public:
	AIconStageActor();

	// 파괴 시 처리 로직
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 무대에 세울 액터를 등록하고 정확한 위치에 배치한다
	void Local_SetupDisplayActor();

	// 아이콘 렌더 타깃 생성
	class UTextureRenderTarget2D* CreateIconRenderTarget(int32 Width, int32 Height);

	// 텍스처 타깃 설정
	void SetTextureTargetAs(class UTextureRenderTarget2D* Target);

	// 현재 설정된 타겟 캡처
	void CaptureTarget();

	// 렌더 타깃 Getter
	class UTextureRenderTarget2D* GetRenderTarget() const;

protected:
	// 아이템 액터를 배치할 위치를 반환한다
	FVector GetItemStageLocation() const;

	// 아이템 액터를 배치할 때 사용할 회전를 반환한다
	FRotator GetItemStageRotation() const;

	// 캡쳐하려는 대상의 XYZ 크기에 따라 카메라 위치를 조정한다
	// 가로 세로만 고려하지 않고 타깃의 두께(Depth)도 고려하는 이유는,
	// 액터 Location이 아니라, 액터의 실질적 끝점으로부터 카메라를 이동시켜야 올바른 거리만큼 이동하기 때문임
	// AspectRatio에는 카메라가 투영될 곳의 가로/세로 값을 전달한다
	void AdjustCameraOnTarget(float TargetDepth, float TargetWidth, float TargetHeight, float AspectRatio);

	UFUNCTION()
	void OnRep_IconRelatives();

protected:
	virtual void BeginPlay() override;

public:
	// 현재 디스플레이 중인 액터
	UPROPERTY(ReplicatedUsing = OnRep_IconRelatives)
	TObjectPtr<class ABaseItem> DisplayedActor = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_IconRelatives)
	TObjectPtr<class UInvObject> ReferencingInvObj = nullptr;

public:
	// 아이콘 디스플레이용 카메라 컴포넌트
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USceneCaptureComponent2D* CaptureComponent = nullptr;

	// 디스플레이할 액터를 소환할 위치 및 방향
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UArrowComponent* DisplayComponent = nullptr;

	// 이 액터의 아이콘 생성이 완료되었는지 여부
	bool bLocal_HasIconGenerated = false;
};
