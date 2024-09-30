// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_EndGrab.h"
#include "Interface_CombatCharacter.h"


void UAnimNotify_EndGrab::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        if (OwnerActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
        {
            IInterface_CombatCharacter::Execute_ExitGrabPose(OwnerActor);
        }
    }
}
