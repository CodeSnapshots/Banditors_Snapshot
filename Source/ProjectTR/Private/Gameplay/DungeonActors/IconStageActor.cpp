// Copyright (C) 2024-2025 by Haguk Kim


#include "DungeonActors/IconStageActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"

#include "Core/TRMacros.h"
#include "Inventory/InvObject.h"
#include "Items/BaseItem.h"

// Sets default values
AIconStageActor::AIconStageActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 아이콘 액터는 특성 상 굉장히 먼 거리에 스폰되는데, 이때도 레플리케이션이 처리되야 한다
	bReplicates = true;
	NetCullDistanceSquared = MAX_FLT;

	if (!DisplayComponent)
	{
		DisplayComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	}

	if (!RootComponent)
	{
		RootComponent = DisplayComponent;
	}

	if (!CaptureComponent)
	{
		CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComp"));

		// 프로퍼티
		CaptureComponent->ProjectionType = ECameraProjectionMode::Type::Orthographic;
		CaptureComponent->FOVAngle = 60.0f;
		CaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList; // 타깃 아이템 액터만 렌더링한다
		CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
		CaptureComponent->CompositeMode = ESceneCaptureCompositeMode::SCCM_Overwrite;
		CaptureComponent->MaxViewDistanceOverride = 1024.f;
		CaptureComponent->DetailMode = EDetailMode::DM_High;

		CaptureComponent->bAutoActivate = false;
		CaptureComponent->bCaptureOnMovement = false;
		CaptureComponent->bCaptureEveryFrame = false;

		CaptureComponent->SetupAttachment(RootComponent);
	}

	SetActorEnableCollision(false);
	bReplicates = true; // 레플리케이션 사용
}

void AIconStageActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 파괴 시 바인딩 된 액터도 함께 파괴
	if (HasAuthority() && IsValid(DisplayedActor))
	{
		DisplayedActor->Destroy();
	}
}

void AIconStageActor::Local_SetupDisplayActor()
{
	if (!IsValid(DisplayedActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Local_SetupDisplayActor - DisplayedActor is invalid!"));
		return;
	}

	if (DisplayedActor)
	{
		DisplayedActor->SetActorLocation(GetItemStageLocation());
		DisplayedActor->SetActorRotation(GetItemStageRotation() + DisplayedActor->IconDisplayRotation);

		// 카메라 조정
		FVector ActorOrigin; // Unused
		FVector ActorExtent;
		ABaseItem* GameItem = Cast<ABaseItem>(DisplayedActor);
		if (GameItem)
		{
			if (GameItem->GetRootComponent() && GameItem->ReachComponent)
			{
				// 리치 컴포넌트 중앙이 카메라 위치에 오도록 맞춘다
				// 아이템 종류에 따라(총기) 이미 맞춰져 있을 수도 있다
				// 메시는 액터 생성 과정을 거치며 리치 컴포넌트에 align 되어 있으므로, 아이콘 위치가 정확하게 표기된다
				FVector RootReachOffset = GameItem->GetRootComponent()->GetComponentLocation() - GameItem->ReachComponent->GetComponentLocation();
				GameItem->SetActorLocation(GameItem->GetActorLocation() + RootReachOffset);
			}

			ActorExtent = GameItem->GetItemSize();
			float AspectRatio = 1.0f;
			if (GameItem->GetInvObject() && GameItem->GetInvObject()->GetDimensions().Y > 0)
			{
				// NOTE:
				// 텍스처 SizeX, SizeY가 제대로 반영되지 않는 오류가 있으므로 어차피 같은 값을 가지는 아이콘 격자 크기를 사용
				AspectRatio = static_cast<float>(GameItem->GetInvObject()->GetDimensions().X) / GameItem->GetInvObject()->GetDimensions().Y;
			}
			AdjustCameraOnTarget(ActorExtent.X, ActorExtent.Y, ActorExtent.Z, AspectRatio);
		}
		else
		{
			DisplayedActor->GetActorBounds(true, ActorOrigin, ActorExtent);
			AdjustCameraOnTarget(ActorExtent.X, ActorExtent.Y, ActorExtent.Z, 1.0f);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Local_SetupDisplayActor - ActorToDisplay is nullptr!"));
	}
}

void AIconStageActor::SetTextureTargetAs(UTextureRenderTarget2D* Target)
{
	CaptureComponent->TextureTarget = Target;
}

void AIconStageActor::CaptureTarget()
{
	// 타깃 액터만 표시하도록 설정
	// 이 과정은 서버 및 클라 모두에게서 처리되어야 하며, 호출되기 전에 사전에 타깃 액터에 대한 정보가 레플리케이션 되어야 한다
	CaptureComponent->ShowOnlyActors = { DisplayedActor };

	// 수동 캡처를 처리하기 위한 과정이 다소 비직관적인데, 다음과 같다
	// 1. Auto Activate, Capture Every Frame false로 설정한 채 생성
	// 2. 캡처 시작 시 Capture Every Frame true로 설정 후 Activate() 호출
	// 3. CaptureSceneDeferred() 호출 - 캡처 처리
	// 4. 캡처가 완료되었다면 Deactivate(); 이때 bCaptureEveryFrame은 다시 false로 돌려놓는 게 아니라, true를 유지해야 한다
	CaptureComponent->bCaptureEveryFrame = true;
	CaptureComponent->Activate();
	CaptureComponent->CaptureSceneDeferred();
	CaptureComponent->Deactivate();
}

UTextureRenderTarget2D* AIconStageActor::GetRenderTarget() const
{
	if (CaptureComponent)
	{
		return CaptureComponent->TextureTarget;
	}
	return nullptr;
}

FVector AIconStageActor::GetItemStageLocation() const
{
	// TODO
	return GetActorLocation();
}

FRotator AIconStageActor::GetItemStageRotation() const
{
	// TODO
	return FRotator(0,0,0);
}

void AIconStageActor::AdjustCameraOnTarget(float TargetDepth, float TargetWidth, float TargetHeight, float AspectRatio)
{
	FVector DefaultRelativeLocaiton = CaptureComponent->GetRelativeLocation();

	float FOVDegrees = CaptureComponent->FOVAngle;
	float FOVRadians = FMath::DegreesToRadians(FOVDegrees);
	float VerticalFOVRadians = 2 * FMath::Atan(FMath::Tan(FOVRadians / 2) / AspectRatio);

	float DistanceFromObjectWidth = TargetWidth / FMath::Tan(FOVRadians / 2.0f);
	float DistanceFromObjectHeight = TargetHeight / FMath::Tan(VerticalFOVRadians / 2.0f);
	float DistanceFromObject = FMath::Max(DistanceFromObjectWidth, DistanceFromObjectHeight);

	CaptureComponent->SetRelativeLocation(
		FVector(
			(DistanceFromObject + (TargetDepth / 2)) * -1, /*중요: 물체의 두께를 고려해주어야 카메라가 올바르게 물체 전체를 담아냄. 음의 방향으로 이동*/
			DefaultRelativeLocaiton.Y, 
			DefaultRelativeLocaiton.Z
		)
	);
	return;
}

void AIconStageActor::OnRep_IconRelatives()
{
	if (ReferencingInvObj && DisplayedActor)
	{
		ReferencingInvObj->Local_InitIconStageActor();
	}
}

void AIconStageActor::BeginPlay()
{
	Super::BeginPlay();


}
