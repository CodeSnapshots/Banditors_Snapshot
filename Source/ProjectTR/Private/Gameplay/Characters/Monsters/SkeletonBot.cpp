// Copyright (C) 2024-2025 by Haguk Kim


#include "Characters/Monsters/SkeletonBot.h"

ASkeletonBot::ASkeletonBot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 소켓 네임의 경우 디폴트 값을 사용
}