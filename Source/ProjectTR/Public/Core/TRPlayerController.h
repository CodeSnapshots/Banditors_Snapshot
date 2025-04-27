// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"

#include "Core/TRMacros.h"
#include "TRPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTR_API ATRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
#pragma region /** Networking */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(ATRPlayerController, CurrDungeonDepth);
	}
#pragma endregion

#pragma region /** Core */
public:
	ATRPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 폰 Possession이 변경될 경우 서버와 클라 모두에게서 호출된다
	UFUNCTION()
	void Local_OnPawnPossessionChange(APawn* OldPosPawn, APawn* NewPosPawn);
#pragma endregion

#pragma region /** Level transition */
public:
	// 서버가 자신 및 모든 클라이언트들에게 레벨 트랜지션 준비 완료상태를 완료할 것을 요청한다
	void Server_ReqHostsLvlTransPrep();

protected:
	// 모든 호스트들이 트랜지션 준비를 위해 호출한다
	// 모든 호스트가 처리 완료해야만 트랜지션이 비로소 처리될 수 있다
	UFUNCTION(Client, Reliable)
	void Host_PrepareLevelTransition();

	// 서버에게 트랜지션 준비를 마쳤음을 알림
	UFUNCTION(Server, Reliable)
	void Server_OnHostLvlTransPrepDone();
#pragma endregion

#pragma region /** Visuals */
public:
	// 로컬에서 바로 RPC를 호출하는 것은 지양할 것
	// Duration이 음수인 경우 1초로 설정됨
	UFUNCTION(Client, Reliable)
	void Local_DrawGlobalPingRPC(UPrimitiveComponent* TargetComp, float Duration, bool bIsServerRequest = false);
	void Local_DrawOutline(UPrimitiveComponent* TargetComp, bool bIsServerRequest);
	void Local_EraseOutline(UPrimitiveComponent* TargetComp, bool bIsServerRequest);

protected:
	// 서버의 요청에 의해 드로잉 중인 아웃라인들을 의미한다
	// 이 컴포넌트들은 로컬 아웃라인 변경의 영향을 받지 않는다
	TSet<TWeakObjectPtr<UPrimitiveComponent>> Local_ServerManagedOutlines;
#pragma endregion

#pragma region /** Widgets */
public:
	// 이 호스트에 대해 로컬 위젯을 생성한다
	// 만약 특정 폰에 바운딩 되어있다면 폰을 인자로 전달한다
	UUserWidget* Local_CreateWidget(TSubclassOf<UUserWidget> WidgetClass, APawn* BoundedTo = nullptr);

	// 위젯을 로컬 뷰포트에 추가한다
	void Local_DisplayWidget(class UUserWidget* Widget, int32 ZOrder = WZO_DEFAULT);

	// 위젯을 화면 상에서 숨긴다(collapse); 이 함수는 뷰포트로부터 위젯을 제거하지는 않는다
	void Local_CollapseWidget(class UUserWidget* Widget);

	// 위젯에 포커싱을 맞추고 인풋을 해당 위젯에 한정되도록 전환한다
	void Local_FocusWidget(class UUserWidget* Widget, bool bShowCursor, bool bFlushKey);

	// 포커스를 게임으로 전환한다
	void Local_FocusGame(bool bShowCursor, bool bFlushKey);

	// 주어진 위젯의 레퍼런스를 모두 제거한다
	void Local_DerefWidget(class UUserWidget* Widget);

	// 해당 캐릭터에 바운딩된 위젯의 레퍼런스를 모두 제거한다
	void Local_DerefPawnBoundedWidgets(class APawn* BoundPawn);

	// 이 호스트 컨트롤러에 바운딩된 위젯의 레퍼런스를 모두 제거한다
	void Local_DerefHostBoundedWidgets();

	// 이 컨트롤러의 소유자에게 해당하는 텍스트를 표기한다
	UFUNCTION(Client, Reliable)
	void Local_AlertTextRPC(const FString& Text, float Duration);

	// 이 컨트롤러의 소유자에게 상점 UI를 표시하고 인터랙션을 시작한다
	UFUNCTION(Client, Reliable)
	void Local_StartShoppingRPC(class ATRShop* ShopLogicActor);

public:
	// Alert를 위해 사용할 수 있는 텍스트 표기 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UTextAlertWidget> TextAlertWidgetClass;

	// 인벤토리 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> InvWidgetClass;

	// 상점 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> ShopWidgetClass;

	// HUD 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	// 게임 정보 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> GameInfoWidgetClass;

	// 어떤 특정 폰의 존재와 위젯의 생명주기가 바운딩 되어있을 경우 이 곳에 등록된다
	// i.e. 인벤토리 위젯은 인벤토리 소유 액터가 사망하면 사라져야 한다
	// Level Persistent - Pawn들은 트랜지션 시 파괴되므로 이 TMap도 자연스럽게 비게 됨
	TMap<APawn*, TSet<UUserWidget*>> PawnBoundWidgets;

	// 호스트 자체와 위젯이 바운딩 되어있을 경우 이 곳에 등록된다
	// Level Persistent
	TArray<UUserWidget*> HostBoundWidgets;
#pragma endregion

#pragma region /** Gameplay */
public:
	// 이 캐릭터가 속한 던전 깊이를 설정한다
	void Server_SetCurrDungeonDepth(int32 Depth);

	// 이 캐릭터가 속한 던전 깊이를 반환한다
	// 레플리케이션 지연 등으로 인해 클라이언트에서는 정확하지 않은 값이 반환될 수 있다
	UFUNCTION(BlueprintCallable)
	int32 Local_GetCurrDungeonDepth();

protected:
	UFUNCTION()
	void OnRep_CurrDungeonDepth();
	void Local_OnCurrDungeonDepthUpdated();

protected:
	// 현재 속한 던전 층계
	// NOTE: 서버에서 Authoritative한 값에 접근하기 위해서는 이 값 대신 게임모드의 값을 사용할 것
	// Level Persistent - Seamless transition이 종료되고 GameMode에서 새 값으로 업데이트함
	UPROPERTY(ReplicatedUsing = OnRep_CurrDungeonDepth)
	int32 CurrDungeonDepth = 0;
#pragma endregion
};
