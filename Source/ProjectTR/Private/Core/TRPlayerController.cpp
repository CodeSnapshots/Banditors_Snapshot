// Copyright (C) 2024-2025 by Haguk Kim


#include "Core/TRPlayerController.h"
#include "Core/TRGameInstance.h"
#include "Core/TRGameState.h"
#include "Core/CustomUtil.h"
#include "Core/ProjectTRGameModeBase.h"
#include "Characters/FPSCharacter.h"
#include "DungeonActors/TRShop.h"
#include "Dungeon/TRDungeonGenerator.h"
#include "UI/ShopBasedWidget.h"
#include "UI/TRHUDWidget.h"
#include "UI/TextAlertWidget.h"

ATRPlayerController::ATRPlayerController()
{
	bReplicates = true;
}

void ATRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 게임 인풋 사용하도록 설정
	SetInputMode(FInputModeGameOnly());

	// 폰 변경 시의 로직 델리게이트 연동
	if (IsLocalController())
	{
		OnPossessedPawnChanged.AddDynamic(this, &ATRPlayerController::Local_OnPawnPossessionChange);
		// 최초 1회 수동 호출
		if (APawn* ControlledPawn = GetPawn())
		{
			Local_OnPawnPossessionChange(nullptr, ControlledPawn);
		}
	}

	// 월드 깊이 저장
	if (HasAuthority() && GetWorld())
	{
		AProjectTRGameModeBase* TRGM = Cast<AProjectTRGameModeBase>(GetWorld()->GetAuthGameMode());
		if (TRGM)
		{
			TRGM->UpdatePlayersDungeonDepth();
		}
	}
}

void ATRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ATRPlayerController::Local_OnPawnPossessionChange(APawn* OldPosPawn, APawn* NewPosPawn)
{
	// NOTE: 서버 로직 필요 시 OnPossess 등 다른 함수 사용할 것
	if (!this->IsLocalController()) return;

	// Null일 수 있다
	if (OldPosPawn)
	{
		// NOTE: 위젯 삭제를 여기서 처리하게 될 경우 순서가 꼬여 문제가 발생한다
		// 위젯은 액터 Destruction 시 호출된다

		AFPSCharacter* OldFPSPawn = Cast<AFPSCharacter>(OldPosPawn);
		if (OldFPSPawn)
		{
			OldFPSPawn->Local_SetAimedTargetUITracking(false);
		}
	}

	if (NewPosPawn)
	{
		AFPSCharacter* NewFPSPawn = Cast<AFPSCharacter>(NewPosPawn);
		if (NewFPSPawn)
		{
			NewFPSPawn->Local_PreCreateWidgets();

			// 새 플레이어가 성공적으로 접속해 Possession을 마쳤으므로 이름 갱신을 요청
			NewFPSPawn->Client_RequestUpdateNames();

			NewFPSPawn->Local_SetAimedTargetUITracking(true);
		}
	}
}

void ATRPlayerController::Server_ReqHostsLvlTransPrep()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_ReqClientsLvlTransPrep - Invalid world!"));
		return;
	}
	if (GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_ReqClientsLvlTransPrep - Authority only!"));
		return;
	}

	// 자기 자신을 포함한 모든 호스트에게 트랜지션 준비 요청
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		ATRPlayerController* PC = Cast<ATRPlayerController>(It->Get());
		if (PC)
		{
			PC->Host_PrepareLevelTransition();
		}
	}
}

void ATRPlayerController::Host_PrepareLevelTransition_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("Host_PrepareLevelTransitionCalled Auth%d"), GetLocalRole() == ROLE_Authority);

	UWorld* World = GetWorld();
	if (World)
	{
		// 서브레벨에 포함된 액터만 파괴한다
		for (TActorIterator<AActor> ActorIt(World); ActorIt; ++ActorIt)
		{
			if (*ActorIt && (*ActorIt)->GetLevel() != World->PersistentLevel)
			{
				(*ActorIt)->Destroy();
			}
		}

		// 서브레벨 액터들 파괴가 바로 적용이 안될 수 있기 때문에 체크 필요
		bool bHasSublevelActors = false;
		for (TActorIterator<AActor> ActorIt(World); ActorIt; ++ActorIt)
		{
			if (ActorIt->GetLevel() != World->PersistentLevel && !(*ActorIt)->HasAnyFlags(RF_FinishDestroyed))
			{
				bHasSublevelActors = true;
			}
		}

		// 던전 제너레이터 파괴 준비
		bool bGeneratorDestroyPrepared = true;
		for (TActorIterator<ATRDungeonGenerator> It(World); It; ++It)
		{
			// 반드시 던전 생성이 완료가 된 상태여야 파괴 준비를 할 수 있음
			if (*It && (*It)->GetProgress() >= 1.0f && !World->IsInSeamlessTravel())
			{
				// 반드시 미리 처리해주어 async 로딩중인 값들을 날려주어야
				// seamless 트랜지션 과정에서 크래시가 발생하지 않음
				(*It)->PrepareDestruction();
				continue;
			}
			else
			{
				bGeneratorDestroyPrepared = false;
			}
		}

		// 모든 조건들이 충족 완료되었는지 확인
		if (!bHasSublevelActors && bGeneratorDestroyPrepared)
		{
			// 완료되었을 경우 서버에 알림
			Server_OnHostLvlTransPrepDone();
		}
		else
		{
			// 실패 시 계속 체크
			GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
				{
					Host_PrepareLevelTransition_Implementation();
				})
			);
			return;
		}
	}
}

void ATRPlayerController::Server_OnHostLvlTransPrepDone_Implementation()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_OnHostLvlTransPrepDone_Implementation - Invalid world!"));
		return;
	}

	AProjectTRGameModeBase* TRGM = World->GetAuthGameMode<AProjectTRGameModeBase>();
	if (!TRGM)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_OnHostLvlTransPrepDone_Implementation - Invalid GameMode!"));
		return;
	}

	TRGM->PreparedHostCount++;
	if (TRGM->PreparedHostCount == TRGM->TotalHostCountCached)
	{
		TRGM->bReadyToTravel = true;
	}
	UE_LOG(LogTemp, Error, TEXT("ServerReadytravel %d/%d, ConfirmedTargetAuth:%d"), TRGM->PreparedHostCount, TRGM->TotalHostCountCached, this->GetLocalRole() == ROLE_Authority);
}

void ATRPlayerController::Local_DrawGlobalPingRPC_Implementation(UPrimitiveComponent* TargetComp, float Duration, bool bIsServerRequest)
{
	if (!TargetComp) return;
	if (!bIsServerRequest)
	{
		UE_LOG(LogTemp, Error, TEXT("Client should avoid calling client RPC directly. Instead, use local function directly. Aborting!"));
		return;
	}
	if (Local_ServerManagedOutlines.Contains(TargetComp)) return;

	Local_DrawOutline(TargetComp, bIsServerRequest);

	if (Duration < 0) Duration = 1.0f;
	FTimerHandle NewPingTimer;
	GetWorldTimerManager().SetTimer(NewPingTimer, [this, TargetComp, bIsServerRequest]()
		{
			this->Local_EraseOutline(TargetComp, bIsServerRequest);
		},
		Duration, false
	);
}

void ATRPlayerController::Local_DrawOutline(UPrimitiveComponent* TargetComp, bool bIsServerRequest)
{
	// 이 호스트가 조작중인 폰이 핑잉 대상인 경우 무시한다
	// 자기 캐릭터에 갑자기 외곽선이 생기는 경우 부자연스럽기 때문
	// 이는 서버 요청이더라도 마찬가지
	APawn* PawnTarget = Cast<APawn>(TargetComp->GetOwner());
	if (PawnTarget && GetPawnOrSpectator() == PawnTarget) return;

	if (Local_ServerManagedOutlines.Contains(TargetComp)) return;
	if (bIsServerRequest)
	{
		Local_ServerManagedOutlines.Add(TargetComp);
	}
	TargetComp->SetRenderCustomDepth(true);
}

void ATRPlayerController::Local_EraseOutline(UPrimitiveComponent* TargetComp, bool bIsServerRequest)
{
	if (!bIsServerRequest && Local_ServerManagedOutlines.Contains(TargetComp)) return;
	if (bIsServerRequest)
	{
		Local_ServerManagedOutlines.Remove(TargetComp);
	}
	TargetComp->SetRenderCustomDepth(false);
}

UUserWidget* ATRPlayerController::Local_CreateWidget(TSubclassOf<UUserWidget> WidgetClass, APawn* BoundedTo)
{
	if (!IsLocalController() || !WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Local_CreateWidget - Invalid widget creation."));
		return nullptr;
	}
	
	UUserWidget* CreatedWidget = CreateWidget(this, WidgetClass);
	if (CreatedWidget)
	{
		if (BoundedTo)
		{
			if (PawnBoundWidgets.Contains(BoundedTo))
			{
				PawnBoundWidgets[BoundedTo].Add(CreatedWidget);
			}
			else
			{
				TSet<UUserWidget*> WidgetSet = TSet<UUserWidget*>();
				WidgetSet.Add(CreatedWidget);
				PawnBoundWidgets.Add(BoundedTo, WidgetSet);
			}
		}
		else
		{
			HostBoundWidgets.Add(CreatedWidget);
		}
		return CreatedWidget;
	}
	return nullptr;
}

void ATRPlayerController::Local_DisplayWidget(UUserWidget* Widget, int32 ZOrder)
{
	if (!Widget) return;
	Widget->AddToViewport(ZOrder);
	Widget->SetVisibility(ESlateVisibility::Visible); // 반드시 명시적으로 호출
}

void ATRPlayerController::Local_CollapseWidget(UUserWidget* Widget)
{
	if (!Widget) return;
	Widget->SetVisibility(ESlateVisibility::Collapsed);
}

void ATRPlayerController::Local_FocusWidget(UUserWidget* Widget, bool bShowCursor, bool bFlushKey)
{
	FInputModeUIOnly InputMode = FInputModeUIOnly();
	InputMode.SetWidgetToFocus(Widget->GetCachedWidget());
	SetInputMode(InputMode);
	if (bFlushKey) FlushPressedKeys();
	SetShowMouseCursor(bShowCursor);
}

void ATRPlayerController::Local_FocusGame(bool bShowCursor, bool bFlushKey)
{
	FInputModeGameOnly InputMode = FInputModeGameOnly();
	SetInputMode(InputMode);
	if (bFlushKey) FlushPressedKeys();
	SetShowMouseCursor(bShowCursor);
}

void ATRPlayerController::Local_DerefWidget(UUserWidget* Widget)
{
	if (!IsValid(Widget)) return;

	// 포커스 있을 경우 해제
	if (Widget->HasUserFocus(this))
	{
		Local_FocusGame(false, true);
	}

	// 위젯 트리에서 제거
	Widget->RemoveFromParent();
	Widget->RemoveFromRoot();

	// 로컬 레퍼런스 제거
	bool bWasRelevant = false;
	for (TPair<APawn*, TSet<UUserWidget*>>& Pair : PawnBoundWidgets)
	{
		bWasRelevant |= (Pair.Value.Remove(Widget) > 0);
	}
	bWasRelevant |= (HostBoundWidgets.Remove(Widget) > 0);
	if (!bWasRelevant)
	{
		UE_LOG(LogTemp, Warning, TEXT("Local_DerefWidget - Tried to dereference a non-bound widget."));
	}
}

void ATRPlayerController::Local_DerefPawnBoundedWidgets(APawn* BoundPawn)
{
	if (!BoundPawn) return;

	// 위젯 트리에서 제거
	TSet<UUserWidget*>* WidgetsRef = PawnBoundWidgets.Find(BoundPawn);
	if (!WidgetsRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Local_DerefPawnBoundedWidgets - Tried to dereference a non-bound pawn's widget"));
		return;
	}
	for (UUserWidget* Widget : *WidgetsRef)
	{
		// 포커스 있을 경우 해제
		if (Widget->HasUserFocus(this))
		{
			Local_FocusGame(false, true);
		}
		Widget->RemoveFromParent();
		Widget->RemoveFromRoot();
	}
	WidgetsRef->Empty();

	// 로컬 레퍼런스 제거
	PawnBoundWidgets.Remove(BoundPawn);
}

void ATRPlayerController::Local_DerefHostBoundedWidgets()
{
	for (UUserWidget* Widget : HostBoundWidgets)
	{
		Local_DerefWidget(Widget);
	}
	HostBoundWidgets.Empty();
}

void ATRPlayerController::Local_StartShoppingRPC_Implementation(ATRShop* ShopLogicActor)
{
	if (!IsLocalController()) return;
	AFPSCharacter* ShoppingCharacter = Cast<AFPSCharacter>(GetCharacter());
	if (!ShoppingCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Local_StartShoppingRPC_Implementation - The controller does not possess AFPSCharacter."));
		return;
	}
	if (!ShopLogicActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Local_StartShoppingRPC_Implementation - ShopActor is Invalid."));
		return;
	}

	UUserWidget* NewWidget = Local_CreateWidget(ShopWidgetClass, ShoppingCharacter);
	UShopBasedWidget* ShopWidget = Cast<UShopBasedWidget>(NewWidget);
	if (ShopWidget)
	{
		// 초기화
		ShopWidget->ShopActor = ShopLogicActor;

		Local_DisplayWidget(ShopWidget, WZO_SHOP);
		Local_FocusWidget(ShopWidget, true, true);
	}
}

void ATRPlayerController::Server_SetCurrDungeonDepth(int32 Depth)
{
	if (!HasAuthority())
	{
		return;
	}
	CurrDungeonDepth = Depth;

	// 서버의 경우 수동 호출
	Local_OnCurrDungeonDepthUpdated();
}

int32 ATRPlayerController::Local_GetCurrDungeonDepth()
{
	return CurrDungeonDepth;
}

void ATRPlayerController::OnRep_CurrDungeonDepth()
{
	Local_OnCurrDungeonDepthUpdated();
}

void ATRPlayerController::Local_OnCurrDungeonDepthUpdated()
{
	// UI 업데이트 (서버,클라)
	UWorld* World = GetWorld();
	if (!World) return;
	ATRGameState* TRGS = World->GetGameState<ATRGameState>();
	if (TRGS && TRGS->GameStateHUD)
	{
		TRGS->GameStateHUD->UpdateDungeonDepth();
	}
}

void ATRPlayerController::Local_AlertTextRPC_Implementation(const FString& Text, float Duration)
{
	if (!IsLocalController()) return; // 서버에서 자기가 보유하지 않은 위젯에 대한 변경사항은 무시된다
	
	UUserWidget* CreatedWidget = Local_CreateWidget(TextAlertWidgetClass);
	UTextAlertWidget* TextAlertWidget = Cast<UTextAlertWidget>(CreatedWidget);
	if (TextAlertWidget)
	{
		// 인터랙션 비활성화
		TextAlertWidget->SetIsFocusable(false);
		TextAlertWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		Local_DisplayWidget(TextAlertWidget, WZO_ALERT);
		// 알림 위젯의 경우 포커싱은 필요하지 않음
		
		TextAlertWidget->SetTextForDuration(Text, Duration);
	}
	else if (CreatedWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Local_AlertTextRPC_Implementation - Created widget is not a valid type. Please check the bounded class type."));
		Local_DerefWidget(CreatedWidget);
	}
}
