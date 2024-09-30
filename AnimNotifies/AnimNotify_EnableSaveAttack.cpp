// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EnableSaveAttack.h"
#include "Interface_Player.h"
#include "FightingProjectCharacter.h"

void UAnimNotify_EnableSaveAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        if (OwnerActor->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
        {
            auto Char = IInterface_Player::Execute_GetPlayerCharacter(OwnerActor);
            Char->bCanSaveAttack = true;
        }
    }
}
