// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_SetIsStaggered.h"
#include "Interface_CombatCharacter.h"

void UAnimNotifyState_SetIsStaggered::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration); 
    
    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        if (OwnerActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
        {
            Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(OwnerActor);

            if (Damagable)
            {
                Damagable->bIsStaggered = true;
            }
        }
    }
}

void UAnimNotifyState_SetIsStaggered::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UAnimNotifyState_SetIsStaggered::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

    if (Damagable)
    {
        Damagable->bIsStaggered = false;
    }
}
