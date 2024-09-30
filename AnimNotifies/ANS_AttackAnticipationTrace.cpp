// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_AttackAnticipationTrace.h"
#include "Interface_Player.h"

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

void UANS_AttackAnticipationTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

}

void UANS_AttackAnticipationTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);
}