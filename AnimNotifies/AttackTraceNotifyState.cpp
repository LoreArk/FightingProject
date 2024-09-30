// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTraceNotifyState.h"
#include "Interface_CombatCharacter.h"
#include "FightingProject/FightingProjectCharacter.h"

void UAttackTraceNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{

    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (bool Char = MeshComp->GetOwner()->Implements<UInterface_CombatCharacter>())
    {
        // Call your function to start the action
        IInterface_CombatCharacter::Execute_HitTraceStart(MeshComp->GetOwner(), DamageSettings, HitTraceSettings);
    }
}

void UAttackTraceNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (bool Char = MeshComp->GetOwner()->Implements<UInterface_CombatCharacter>())
    {
        // Call your function to start the action
        IInterface_CombatCharacter::Execute_HitTraceTick(MeshComp->GetOwner());
    }
}

void UAttackTraceNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (bool Char = MeshComp->GetOwner()->Implements<UInterface_CombatCharacter>())
    {
        // Call your function to start the action
        IInterface_CombatCharacter::Execute_HitTraceEnd(MeshComp->GetOwner());
    }
}
