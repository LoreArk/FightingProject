// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_TriggerDeathIfDead.h"
#include "Interface_CombatCharacter.h"
#include "Component_Damagable.h"

void UAnimNotify_TriggerDeathIfDead::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner())
	{
		if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
		{
			UComponent_Damagable* DamagableComponent = IInterface_CombatCharacter::Execute_GetDamagableComponent(MeshComp->GetOwner());
			if (DamagableComponent)
			{
				if (DamagableComponent->bIsDead)
				{
					DamagableComponent->OnDeath.Broadcast(MeshComp->GetOwner());
				}
			}
		}
	}
}
