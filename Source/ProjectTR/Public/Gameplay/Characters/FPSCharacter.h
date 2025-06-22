// Copyright (C) 2024-2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "InputActionValue.h"

#include "Core/TRStructs.h"
#include "DataAssets/CamShakeConfig.h"
#include "Characters/GameCharacter.h"
#include "Characters/Components/FPSCameraComponent.h"
#include "Characters/Components/ExpComponent.h"
#include "FPSCharacter.generated.h"

class UInputMappingContext;
class UCapsuleComponent;

UCLASS()
class PROJECTTR_API AFPSCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	AFPSCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual FGameCharacterInstanceData Server_GetInstanceData();
	virtual bool Server_RestoreFromInstanceData(const FGameCharacterInstanceData& InstData) override;
	virtual void Server_OnDamageInflictedToTarget(AGameCharacter* Target, FDamageEvent const& DamageEvent, int32 Damage, bool bIsKillshot, bool bIsCrit) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

	// 플레이어 데미지 딜링 성공 시 해당 호스트의 로컬 로직 처리
	// 데미지 넘버, 크로스헤어 효과, 히트사운드 등 FX를 처리한다
	UFUNCTION(Client, Reliable)
	void Local_OnPlayerDamageInflictedToTarget(AGameCharacter* Target, FVector DamageLocation, int32 Damage, bool bIsKillshot, bool bIsCrit);

#pragma region /** Action */
protected:
/* 캐릭터 조작*/
	// 캐릭터 이동
	void Move(const FInputActionValue& Value);

	// 달리기
	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);

	// 캐릭터 1인칭 카메라 회전
	void Look(const FInputActionValue& Value);

	// 점프
	void ApplyJump(const FInputActionValue& Value);

	// 공격 (주 행동)
	void Attack(const FInputActionValue& Value);

	// 공격 2 (보조 행동)
	void Attack2(const FInputActionValue& Value);

	// 공격 중단 (주 행동)
	void AttackStop(const FInputActionValue& Value);

	// 공격 2 중단 (보조 행동)
	void Attack2Stop(const FInputActionValue& Value);

	// 앉기
	void Duck(const FInputActionValue& Value);

	// 디버그 액션
	void DebugAction(const FInputActionValue& Value);

	// 도발
	void Taunt(const FInputActionValue& Value);

	// 구르기
	void Roll(const FInputActionValue& Value);

	// 점프 가능 시점 판정 로직 수정
	bool CanJumpInternal_Implementation() const override;

/* 물체 상호작용 */
	// 기본 상호작용
	void Interact(const FInputActionValue& Value);

/* 시스템 */
	// 인벤토리
	void AccessInventory(const FInputActionValue& Value);

	// 현재 게임 정보
	void CheckGameInfo(const FInputActionValue& Value);
	void StopCheckGameInfo(const FInputActionValue& Value);

	// 무기 혹은 도구 변경 인풋 입력 처리
	void SwitchTo0(const FInputActionValue& Value); // Unarmed
	void SwitchTo1(const FInputActionValue& Value);
	void SwitchTo2(const FInputActionValue& Value);
	void SwitchTo3(const FInputActionValue& Value);

	// 시점 전환
	void ToggleViewPerspective(const FInputActionValue& Value);

	// 핑
	void PingAtAim(const FInputActionValue& Value);
#pragma endregion

#pragma region /** Networking */
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(AFPSCharacter, ExpComp);
		DOREPLIFETIME_CONDITION(AFPSCharacter, LastSyncedLocalData, COND_SkipOwner);
	}

public:
	// 상점 트랜잭션 RPC
	// NOTE: 서버사이드에서 RPC 호출을 클라가 요청하기 위해서는 RPC의 주체가 반드시 클라이언트에게 Owned 되어야 한다
	// 즉 ATRShop 내에 RPC를 구현하는 대신, 폰을 경유해 RPC를 호출한다
	UFUNCTION(Server, Reliable)
	void Server_RegisterShopPurchase(class ATRShop* ShopActor, class UInvObject* TradeItem);

	UFUNCTION(Server, Reliable)
	void Server_RegisterShopSell(class ATRShop* ShopActor, class UInvObject* TradeItem);
#pragma endregion

#pragma region /** Aim */
public:
	// 폰에 정의된 기본값은 액터의 위치와 카메라의 회전을 사용하지만, FPSCharacter는 카메라의 위치와 카메라의 회전을 사용한다
	virtual void GetActorEyesViewPoint(FVector& out_Location, FRotator& out_Rotation) const override;

	// 캐릭터의 모델에서 손을 사용하는 동작을 처리 시 기본으로 삼을 위치와 방향을 반환한다
	virtual TPair<FVector, FRotator> GetHandPointInfo() override;

	// 카메라 시야 시작점과 방향을 기준으로 조준 정보를 반환한다
	virtual TPair<FVector, FRotator> GetMuzzleInfo() override;

	UFUNCTION(BlueprintCallable)
	float GetAuthControllerPitch() const;

protected:
	// 컨트롤러 pitch 등 동기화가 필요한 값을 서버로 보내 동기화한다
	UFUNCTION(Server, Unreliable)
	void Server_SyncLocalData(FLocalSyncData Data);

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	FLocalSyncData LastSyncedLocalData;

private:
	UPROPERTY(EditDefaultsOnly)
	float Local_LocalDataSyncInterval = TR_DEFAULT_CONTROLLER_PITCH_SYNC_RATE;
	float Local_TimeSinceLastSync = 0.0f;
#pragma endregion

#pragma region /** Camera */
public:
	// 1인칭 카메라
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<class UFPSCameraComponent> FPSCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<class USpringArmComponent> FPSSpringArm = nullptr;

	// 3인칭 카메라
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<class UFPSCameraComponent> TPSCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<class USpringArmComponent> TPSSpringArm = nullptr;

	// 관전 카메라
	// NOTE: TPSCamera를 겸용으로 쓰지 않는 이유는 캡슐에 어태치할 경우 jitter가 발생하기 때문
	// 그렇다고 디태치하게 되면, 리슨서버에서 슈팅 연산을 처리할 때 오차 발생
	// 따라서 불가피하게 별도의 컴포넌트를 사용
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<class UFPSCameraComponent> SpecCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<class USpringArmComponent> SpecSpringArm = nullptr;

protected:
	// 카메라 셰이크 에셋
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<UCamShakeConfig> CamShakeConfig = nullptr;

	// 현재 카메라 시점
	bool bHost_IsThirdPersonView = false;

public:
	// 현재 사용중인 뷰 카메라
	class UFPSCameraComponent* Host_GetCurrViewCamera() const;

	// 카메라 셰이크 재생
	void Local_PlayCameraShake(TSubclassOf<class UTRCameraShake> CamShake, float Scale);

	// 카메라 시점 변경
	UFUNCTION(Server, Reliable)
	void Server_ToggleCameraViewPerspective();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCameraViewPerspective(bool bIsThirdPerson);

	void Local_SetCameraViewPerspective(bool bIsThirdPerson);

	// 관전 시 로컬 단위에서만 관전 카메라를 활성화한다
	void Local_OnSpectationStateChanged(bool bLocal_Spectating);
#pragma endregion

#pragma region /** Input */
public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 로컬 플레이어에게 InputMappingContext를 추가한다
	void AddLocalPlayerInputMappingContext(const UInputMappingContext* Context, int32 Priority, bool bClearAllMappings);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputMappingContext> DefaultInputMapping = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputConfig> InputConfig = nullptr;

	// 기본 회전 감도
	UPROPERTY(EditDefaultsOnly)
	float RotationSensitivity = 0.15f;
#pragma endregion

#pragma region /** Gameplay */
/* Interaction */
protected:
	// 리치 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Reach")
	float ReachDistance = 1200.0f;

	// 핑 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Reach")
	float PingDistance = 20000.0f;

public:
	// 리치가 닿는 상호작용 대상 판정
	virtual TArray<FHitResult> Reach() override;

	// 리치가 닿는 아이템 판정
	virtual class ABaseItem* ReachItem() override;

	// 픽업 직전 처리해야 하는 로직
	virtual void ProcessBeforeItemPickup(class ABaseItem* ReachedItem) override;

/* Life */
protected:
	// 서버의 사망 시 게임 로직 처리
	virtual void Server_ProcessDeath() override;

	// 멀티캐스트 사망 시 게임 로직 처리
	virtual void Multicast_ProcessDeath() override;

	// 영혼 아이템을 주어진 위치에 생성한다
	void Server_SpawnSoulAt(FVector Location, FRotator Rotation);

	// 데미지 처리 오버라이딩
	// 플레이어 캐릭터는 데미지를 받을경우 일시적으로 이동속도가 감소
	virtual float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// 데미지 받았을 경우의 로컬 로직
	// Unreliable이므로 시각효과 외의 로직이 추가되어서는 안됨
	UFUNCTION(Client, Unreliable/* 성능 위함 */)
	void Local_OnDamageTaken(float Damage, FVector DamageLocation);

protected:
	// 사망 시 드랍할 영혼 아이템 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	TSubclassOf<class ATRSoul> DropSoulItemClass = nullptr;

/* Experience */
public:
	// 경험치 컴포넌트
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Experience")
	TObjectPtr<class UExpComponent> ExpComp = nullptr;

public:
	// 레벨업 시 캐릭터에 대해 처리할 로직을 수행
	void Server_OnLevelUp(int32 NewLevel);

protected:
	// 레벨업 시 레벨업에 성공한 호스트 로컬에서 수행할 로직
	UFUNCTION(Client, Reliable)
	void Client_OnLevelUpRPC(int32 NewLevel);

/* Dungeon */
protected:
	// 룸에 진입하거나 탈출할 경우의 로직 오버라이드
	virtual void Server_OnRoomEnter(class ARoomLevel* RoomLevel) override;
	// NOTE: 필요 시 Exit도 오버라이드 가능
#pragma endregion

#pragma region /** Widgets */
// NOTE:  모든 위젯 관련 기능은 로컬 단위에서 작동한다.
protected:
/* 네임태그 */
	// 네임태그 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<class UWidgetComponent> NameWidgetComponent;

private:
/* 에임 타깃 UI */
	// 주기마다 업데이트
	float Local_AimedTargetUIUpdateRate = 0.066f;

	FTimerHandle AimedTargetUITimer;
	FCollisionQueryParams AimedTargetUICollisionParams;
	FCollisionObjectQueryParams AimedTargetUIObjQueryParams;

	// 이전 사이클에 에임중이었던 액터의 캐시
	TWeakObjectPtr<AActor> PrevAimedActor = nullptr;

public:
	// 에임 기반 UI 활성화 여부를 수정한다
	void Local_SetAimedTargetUITracking(bool bActivate);

	// 네임태그 비지빌리티 설정
	void Local_SetNameWidgetVisibility(bool bVisible);

	// 일부 위젯의 경우 캐릭터 생성과 함께 미리 생성할 필요가 있다
	void Local_PreCreateWidgets();

	// 이 캐릭터의 네임태그에 표시되는 이름을 변경한다
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SetNameTagText(const FString& NewName);

	// 서버에게 모든 플레이어 이름 갱신을 요청
	UFUNCTION(Server, Reliable)
	void Client_RequestUpdateNames();

	// 현재 Deploy중인 무기의 장탄을 문자열 형태로 반환한다
	// 만약 Deploy중인 무기가 없거나, GunItem이 아닐 경우(탄약의 개념이 없을 경우) 빈 문자열을 반환한다
	UFUNCTION(BlueprintCallable)
	FString Host_GetCurrWeaponAmmoLeft() const;

	// 현재 네임태그에 등록된 이름을 문자열로 반환한다
	FString Host_GetName() const;

protected:
	// 크로스헤어 타겟팅 대상의 UI 렌더 처리
	UFUNCTION()
	void Local_UpdateAimedTargetUI();

	// 강조 대상에 대한 적합한 처리 진행
	void Local_HighlightTarget(AActor* Target, bool bVisibility, bool bIgnoreCharacters);

/* 핑 */
private:
	FTimerHandle AimedTargetPingTimer;
	FCollisionQueryParams AimedTargetPingCollisionParams;
	FCollisionObjectQueryParams AimedTargetPingObjQueryParams;

protected:
	// 현재 에임중인 대상에 대한 핑잉을 처리한다
	UFUNCTION(Server, Reliable)
	void Server_PingAtAim();

/* 플레이어 정보 */
public:
	// NOTE: 아래 함수들은 가급적 AllyStateWidget에 의해서만 호출되어야 한다
	void BindAllyState(class UAllyStateWidget* AllyState);
	void UnbindAllyState();

private:
	// 현재 이 캐릭터에 바인딩된 ally state UI
	TWeakObjectPtr<class UAllyStateWidget> Local_AllyStateWidget = nullptr;

	// 게임 정보 위젯
	// 토글 해제될 경우 파괴된다
	// NOTE: UTeamStateWidget 외의 정보가 필요한 경우 교체 가능
	// NOTE: 토글 시 파괴하는 대신 캐싱하는 구조로 변경 가능 
	TWeakObjectPtr<class UTeamStateWidget> Local_GameInfoWidget = nullptr;
#pragma endregion

#pragma region /** Spectator */
public:
	// 컨트롤러를 관전자 폰으로 옮겨 관전 모드에 들어간다
	void Server_StartSpectating();

protected:
	// 관전 시 사용할 폰
	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TSubclassOf<class ATRSpectatorPawn> SpecPawnClass;
#pragma endregion

#pragma region /** VFX */
public:
	// 슬라이딩 시 재생할 VFX
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	class UNiagaraSystem* SlideVFX = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	class UNiagaraComponent* SlideVFXComp = nullptr;

public:
	virtual void Local_SetSlideFx(bool bValue) override;
#pragma endregion
};
