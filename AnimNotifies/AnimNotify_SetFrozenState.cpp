// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SetFrozenState.h"
#include "Interface_CombatCharacter.h"
#include "Component_Damagable.h"


void UAnimNotify_SetFrozenState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        if (OwnerActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
        {
            if (UComponent_Damagable* Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(OwnerActor))
            {
                Damagable->CurrentFrozenState = FrozenState;
            }
        }
    }

}
