// Copyright (C) 2024-2025 by Haguk Kim


#include "Animation/ANS_OnMeleeAtk.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Core/TRMacros.h"
#include "Core/TRUtils.h"
#include "Core/TRCVar.h"
#include "Characters/GameCharacter.h"
#include "Characters/BotCharacter.h"

UANS_OnMeleeAtk::UANS_OnMeleeAtk()
{
	//HitObjectTypes.Add()
}

void UANS_OnMeleeAtk::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	// 매번 초기화
	AGameCharacter* Attacker = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (!IsValid(Attacker) || !Attacker->HasAuthority()) return;
	Attacker->HitTargetsForCurrSequence.Empty();
}

void UANS_OnMeleeAtk::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	TrackMeleeTargets(MeshComp);
}

void UANS_OnMeleeAtk::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGameCharacter* Attacker = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (!IsValid(Attacker) || !Attacker->HasAuthority()) return;

	// 모든 대상들에 대해 밀리 로직 수행
	TSet<AGameCharacter*> HitTargets;
	for (const TPair<AGameCharacter*, FHitResult>& Pair : Attacker->HitTargetsForCurrSequence)
	{
		// 중복 히트 시 첫번째 히트만을 처리함
		if (HitTargets.Contains(Pair.Get<0>())) continue;
		Attacker->ProcessMeleeAtk(Pair.Get<0>(), Pair.Get<1>());
		HitTargets.Add(Pair.Get<0>());
	}

	Attacker->bPrevMeleeTickLocationValid = false;
}

void UANS_OnMeleeAtk::TrackMeleeTargets(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp) return;
	AGameCharacter* Attacker = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (!IsValid(Attacker) || !Attacker->HasAuthority()) return;

	FName MeleeSockName;
	if (bIsRightHandMelee) MeleeSockName = TEXT(MELEE_ATK_SOCKET_R);
	else MeleeSockName = TEXT(MELEE_ATK_SOCKET_L);
	FVector SocketLocation = MeshComp->GetSocketLocation(MeleeSockName);

	if (!Attacker->bPrevMeleeTickLocationValid)
	{
		Attacker->PrevMeleeTickLocation = SocketLocation;
		Attacker->bPrevMeleeTickLocationValid = true;
		return;
	}

	FHitResult MeleeRes;
	bool bDrawDebug = false;
#if WITH_EDITOR
	if (CVarShowDebugShapes.GetValueOnAnyThread())
	{
		bDrawDebug = true;
	}
#endif
	bool bHit = Attacker->TraceMelee(MeleeRes, Attacker->PrevMeleeTickLocation, SocketLocation, bDrawDebug);
	if (bHit)
	{
		AGameCharacter* HitTarget = Cast<AGameCharacter>(MeleeRes.GetActor());
		if (!HitTarget || !Attacker || TRUtils::IsAllyWith(HitTarget, Attacker)) return; // 아군 무시
		Attacker->HitTargetsForCurrSequence.Add(HitTarget, MeleeRes);
	}
}
