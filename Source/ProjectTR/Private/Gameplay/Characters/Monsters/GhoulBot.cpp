// Copyright (C) 2025 by Haguk Kim


#include "Gameplay/Characters/Monsters/GhoulBot.h"

AGhoulBot::AGhoulBot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 소켓 네임의 경우 디폴트 값을 사용
}
