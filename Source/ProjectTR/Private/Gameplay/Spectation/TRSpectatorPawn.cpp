// Copyright (C) 2024-2025 by Haguk Kim


#include "Spectation/TRSpectatorPawn.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/SphereComponent.h"

#include "Core/ProjectTRGameModeBase.h"
#include "Core/TRPlayerController.h"
#include "Core/TRCVar.h"
#include "Characters/GameCharacter.h"
#include "Characters/FPSCharacter.h"
#include "Characters/TRPlayerState.h"
#include "DataAssets/InputConfig.h"

ATRSpectatorPawn::ATRSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bHighPriority = false;
	SetActorEnableCollision(true);
}

void ATRSpectatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FBox ATRSpectatorPawn::GetComponentsBoundingBox(bool bNonColliding, bool bIncludeFromChildActors) const
{
	// 관전 폰의 바운딩 박스는 NonColliding 플래그가 true여야 함;
	// 관전 폰 자체가 콜리전을 처리하지 않을 수 있기 때문에 이렇게 하지 않을 경우 룸 컬링이 제대로 처리되지 않을 수 있음
	return Super::GetComponentsBoundingBox(true, false);
}

void ATRSpectatorPawn::Server_ChangeSpecTargetRPC_Implementation(bool Direction)
{
	if (!HasAuthority()) return;

	ACharacter* OriginTarget = SpectatingTarget;
	ACharacter* FirstValidTarget = nullptr; // 가장 인덱스가 작은 유효 타깃
	ACharacter* LastValidTarget = nullptr; // 가장 인덱스가 큰 유효 타깃
	ACharacter* TargetNextOrigin = nullptr;
	ACharacter* TargetPrevOrigin = nullptr;
	int OriginTargetIdx = -1;

	UWorld* World = GetWorld();
	if (World)
	{
		AProjectTRGameModeBase* GameMode = Cast<AProjectTRGameModeBase>(World->GetAuthGameMode());
		if (GameMode)
		{
			TArray<ATRPlayerController*> PlayersConnected = GameMode->GetPlayersConnected();
			if (PlayersConnected.IsEmpty())
			{
				Server_SetSpectatingTarget(nullptr);
				return;
			}
			for (int Index = 0; Index < PlayersConnected.Num(); ++Index)
			{
				ATRPlayerController* PlayerController = PlayersConnected[Index];

				// 게임 세션에 참여중이 아닌 경우 제외
				ATRPlayerState* TRPlayerState = PlayerController->GetPlayerState<ATRPlayerState>();
				if (!IsValid(TRPlayerState) || TRPlayerState->GetIsOut()) continue;

				// 관전 폰은 무시하고, 조작 중인 폰을 가져온다
				ACharacter* TargetCharacter = PlayerController->GetCharacter();
				if (Server_IsTargetSpectatable(TargetCharacter))
				{
					// 헤드, 테일 추적
					if (!FirstValidTarget) FirstValidTarget = TargetCharacter;
					LastValidTarget = TargetCharacter;

					// 기존 타깃인지 확인
					if (OriginTarget == TargetCharacter)
					{
						OriginTargetIdx = Index;
					}
					else
					{
						// 타깃 위치를 아직 못찾았을 경우 타깃 이전에 있다고 간주하고 캐싱
						if (OriginTargetIdx < 0) TargetPrevOrigin = TargetCharacter;
						else if (!TargetNextOrigin) TargetNextOrigin = TargetCharacter;
					}
				}
			}
		}
	}

	if (OriginTargetIdx < 0)
	{
		// NOTE: 관전 가능 대상이 없을 경우 여기서 nullptr가 전달됨
		Server_SetSpectatingTarget(FirstValidTarget);
	}
	else
	{
		if (Direction)
		{
			if (TargetNextOrigin)
			{
				Server_SetSpectatingTarget(TargetNextOrigin);
			}
			else
			{
				Server_SetSpectatingTarget(FirstValidTarget);
			}
		}
		else
		{
			if (TargetPrevOrigin)
			{
				Server_SetSpectatingTarget(TargetPrevOrigin);
			}
			else
			{
				Server_SetSpectatingTarget(LastValidTarget);
			}
		}
	}

	return;
}

void ATRSpectatorPawn::OnRep_SpecTargetChange()
{
	Local_OnSpecTargetChange();
}

void ATRSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Super 호출 불필요
	// 인풋 맵핑 등록
	AddLocalPlayerInputMappingContext(DefaultInputMapping, TR_SPECPAWN_DEFAULT_INPUTMAPPING_PRIORITY, true);

	// 인풋 액션 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InputConfig->MoveInputAction, ETriggerEvent::Triggered, this, &ATRSpectatorPawn::Move);
		EnhancedInputComponent->BindAction(InputConfig->LookInputAction, ETriggerEvent::Triggered, this, &ATRSpectatorPawn::Look);
		EnhancedInputComponent->BindAction(InputConfig->DescendAction, ETriggerEvent::Triggered, this, &ATRSpectatorPawn::Fall);
		EnhancedInputComponent->BindAction(InputConfig->AscendAction, ETriggerEvent::Triggered, this, &ATRSpectatorPawn::Float);

		EnhancedInputComponent->BindAction(InputConfig->AttackAction, ETriggerEvent::Triggered, this, &ATRSpectatorPawn::SpecChangeNext);
		EnhancedInputComponent->BindAction(InputConfig->Attack2Action, ETriggerEvent::Triggered, this, &ATRSpectatorPawn::SpecChangePrev);
	}
}

void ATRSpectatorPawn::AddLocalPlayerInputMappingContext(const UInputMappingContext* Context, int32 Priority, bool bClearAllMappings)
{
	// 인풋 맵핑 여러 개 등록 시 하나의 인풋에 대해 우선순위 높은 순으로 확인하며 처리한다
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (Context)
			{
				if (bClearAllMappings) InputSystem->ClearAllMappings();
				InputSystem->AddMappingContext(Context, Priority);
			}
		}
	}
	return;
}

void ATRSpectatorPawn::Move(const FInputActionValue& Value)
{
	if (!bLocal_EnableMoveInput) return;
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		if (MoveValue.Y != 0.f)
		{
			const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(Direction, MoveValue.Y);
		}

		if (MoveValue.X != 0.f)
		{
			const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(Direction, MoveValue.X);
		}
	}
}

void ATRSpectatorPawn::Look(const FInputActionValue& Value)
{
	if (!bLocal_EnableLookInput) return;
	if (Controller != nullptr)
	{
		const FVector2D LookValue = Value.Get<FVector2D>() * RotationSensitivity;

		if (LookValue.X != 0.f)
		{
			AddControllerYawInput(LookValue.X);
		}

		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y * -1.0f);
		}
	}
}

void ATRSpectatorPawn::Float(const FInputActionValue& Value)
{
	if (!bLocal_EnableMoveInput) return;
	if (Value.Get<bool>())
	{
		AddMovementInput(FVector::UpVector, FloatingSpeed);
	}
}

void ATRSpectatorPawn::Fall(const FInputActionValue& Value)
{
	if (!bLocal_EnableMoveInput) return;
	if (Value.Get<bool>())
	{
		AddMovementInput(FVector::DownVector, FallSpeed);
	}
}

void ATRSpectatorPawn::SpecChangeNext(const FInputActionValue& Value)
{
	Server_ChangeSpecTargetRPC(true);
}

void ATRSpectatorPawn::SpecChangePrev(const FInputActionValue& Value)
{
#if WITH_EDITOR
	if (CVarEnableDebugFeatures.GetValueOnGameThread())
	{
		Server_RespawnPlayerForDebugRPC();
		return;
	}
#endif
	Server_ChangeSpecTargetRPC(false);
}

void ATRSpectatorPawn::Server_RespawnPlayerForDebugRPC_Implementation()
{
	if (!HasAuthority()) return;
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_RespawnPlayerForDebugRPC - Invalid world!"));
		return;
	}
	AProjectTRGameModeBase* GameMode = Cast<AProjectTRGameModeBase>(World->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_RespawnPlayerForDebugRPC - Invalid game mode!"));
		return;
	}
	ATRPlayerState* TRPS = GetPlayerState<ATRPlayerState>();
	if (!TRPS)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_RespawnPlayerForDebugRPC - Invalid player state!"));
		return;
	}

	TPair<TSubclassOf<AFPSCharacter>, FGameCharacterInstanceData*> CachedInstData = TRPS->Server_GetCachedPlayerInstanceData();
	GameMode->RespawnPlayer(Cast<ATRPlayerController>(GetController()), GetTransform()/* 폰 위치에 생성 */, CachedInstData.Get<0>(), *CachedInstData.Get<1>(), true);
	return;
}

void ATRSpectatorPawn::Server_SetSpectatingTarget(ACharacter* NewTarget)
{
	if (!Server_IsTargetSpectatable(NewTarget))
	{
		PrevSpectatingTarget = SpectatingTarget;
		SpectatingTarget = nullptr;
	}
	else
	{
		// 타깃도 동일하고, 관전 가능 여부에도 변함이 없으면 아무 것도 수행하지 않음
		if (SpectatingTarget == NewTarget) return;

		PrevSpectatingTarget = SpectatingTarget;
		SpectatingTarget = NewTarget;
	}

	// 서버의 경우 수동 호출
	Local_OnSpecTargetChange();
}

void ATRSpectatorPawn::Local_OnSpecTargetChange()
{
	APlayerController* SpecController = Cast<APlayerController>(GetController());
	if (!SpecController)
	{
		UE_LOG(LogTemp, Error, TEXT("Local_OnSpecTargetChange - Invalid controller!"));
		return;
	}
	if (!IsValid(SpectatingTarget))
	{
		SpecController->SetViewTargetWithBlend(this);

		bLocal_EnableMoveInput = true;
		bLocal_EnableLookInput = true;
	}
	else
	{
		SpecController->SetViewTargetWithBlend(SpectatingTarget);

		bLocal_EnableMoveInput = false;
		bLocal_EnableLookInput = false;
	}
	Local_OnViewTargetSet();
}

void ATRSpectatorPawn::Local_OnViewTargetSet()
{
	AFPSCharacter* PrevFPSChar = Cast<AFPSCharacter>(PrevSpectatingTarget);
	AFPSCharacter* CurrFPSChar = Cast<AFPSCharacter>(SpectatingTarget);

	if (PrevFPSChar) PrevFPSChar->Local_OnSpectationStateChanged(false);
	if (CurrFPSChar) CurrFPSChar->Local_OnSpectationStateChanged(true);
}

bool ATRSpectatorPawn::Server_IsTargetSpectatable(TWeakObjectPtr<ACharacter> Target)
{
	if (!Target.IsValid()) return false;
	AFPSCharacter* FPSTarget = Cast<AFPSCharacter>(Target);
	if (!IsValid(FPSTarget) || FPSTarget->GetHasDied())
	{
		return false;
	}
	return true;
}
