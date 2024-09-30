// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_SetInvincibility.h"
#include "Component_Damagable.h"
#include "EnemyBase.h"

void UAnimNotifyState_SetInvincibility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

}

void UAnimNotifyState_SetInvincibility::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    AActor* OwnerActor = MeshComp->GetOwner();


    if (OwnerActor)
    {
        if (OwnerActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
        {
            if (UComponent_Damagable* Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(OwnerActor))
            {
                Damagable->bIsInvincible = true;
            }
        }
    }
}

void UAnimNotifyState_SetInvincibility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);

    AActor* OwnerActor = MeshComp->GetOwner();


    if (OwnerActor)
    {
        if (OwnerActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
        {
            if (UComponent_Damagable* Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(OwnerActor))
            {
                Damagable->bIsInvincible = false;
            }
        }
    }
}