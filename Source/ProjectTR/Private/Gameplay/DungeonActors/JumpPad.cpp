// Copyright (C) 2025 by Haguk Kim


#include "DungeonActors/JumpPad.h"
#include "Components/ArrowComponent.h"

#include "Characters/GameCharacter.h"
#include "Characters/Components/BaseCharacterMovementComponent.h"

AJumpPad::AJumpPad()
{
}

void AJumpPad::ProcessCharacterOverlap(AGameCharacter* Target)
{
	if (!Target) return;
	UBaseCharacterMovementComponent* TRCMC = Target->GetTRCharacterMovementComponent();
	if (!TRCMC) return;

	// 슬라이드가 아닌 크라우치의 경우 무시
	// 착지를 위해 사용 가능
	if (Target->bIsCrouched && !TRCMC->GetIsSliding()) return;

	// 점프 패드의 경우 완전히 서버 주체적인 로직이므로 client prediction을 사용하지 않음
	Target->LaunchCharacter(GetActorUpVector().GetSafeNormal() * JumpSpeed, bOverrideXYVelocity, bOverrideZVelocity);
}

