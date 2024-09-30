// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_GetUpTransition.h"
#include "Component_Damagable.h"
#include "EnemyBase.h"

void UAnimNotifyState_GetUpTransition::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

}

void UAnimNotifyState_GetUpTransition::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
    
}

void UAnimNotifyState_GetUpTransition::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

    AActor* OwnerActor = MeshComp->GetOwner();

    
    if (OwnerActor)
    {
        if (OwnerActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
        {
            if (UComponent_Damagable* Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(OwnerActor))
            {
                //GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "GET UP TRANSITION");

                Damagable->CurrentFrozenState = EFrozenState::None;
            }
        }
    }
}
