// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_AttackAnticipationTrace.h"
#include "Interface_Player.h"

//This anim notify is used to set the frame of the attack montage that makes the enemy AI aware of the incoming attack, and trigger the AI reaction (block, dodge or nothing)

void UANS_AttackAnticipationTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        if (OwnerActor->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
        {
             IInterface_Player::Execute_AttackAnticipationTrace(OwnerActor);
        }
    }
}
