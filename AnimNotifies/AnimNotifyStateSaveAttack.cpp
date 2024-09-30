// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyStateSaveAttack.h"
#include "FightingProject/FightingProjectCharacter.h"


void UAnimNotifyStateSaveAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (AFightingProjectCharacter* Character = Cast<AFightingProjectCharacter>(MeshComp->GetOwner()))
    {
        // Call your function to start the action
        //Character->AttackTraceNotifyBegin();
    }
}

void UAnimNotifyStateSaveAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (AFightingProjectCharacter* Character = Cast<AFightingProjectCharacter>(MeshComp->GetOwner()))
    {
        // Call your function to start the action
        Character->HandleAttackCombo(FrameDeltaTime);
    }
}

void UAnimNotifyStateSaveAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (AFightingProjectCharacter* Character = Cast<AFightingProjectCharacter>(MeshComp->GetOwner()))
    {
        // Call your function to start the action
        //Character->EndAttackCombo();
    }
}
