// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_LockYawControl.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "FightingProject/FightingProjectCharacter.h"


void UAnimNotifyStateLockYawControl::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (AFightingProjectCharacter* Character = Cast<AFightingProjectCharacter>(MeshComp->GetOwner()))
    {
        // Call your function to start the action
        Character->DisableYawControl();
    }
}

void UAnimNotifyStateLockYawControl::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (AFightingProjectCharacter* Character = Cast<AFightingProjectCharacter>(MeshComp->GetOwner()))
    {
        // Call your function to start the action
        Character->DisableYawControl();
    }
}

void UAnimNotifyStateLockYawControl::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (AFightingProjectCharacter* Character = Cast<AFightingProjectCharacter>(MeshComp->GetOwner()))
    {
        Character->EnableYawControl();
    }
}
