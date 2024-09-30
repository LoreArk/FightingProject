// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_AnimationDoDamage.h"
#include "CombatCharacter.h"
#include "Interface_Finishers.h"
#include "Interface_Damagable.h"
#include "Interface_CombatCharacter.h"

void UAnimNotify_AnimationDoDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();

	if (Owner && Owner->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
	{
		if (auto Char = IInterface_CombatCharacter::Execute_GetCombatCharacter(Owner))
		{
			AActor* Victim = IInterface_Finishers::Execute_GetFinisherVictim(MeshComp->GetOwner());

			if (Victim)
			{
				
				Char->OnAnimationDoDamage.Broadcast();
				DamageSettings.ChargedAttackPercentage = 1.f;
				IInterface_Damagable::Execute_TakeDamage(Victim, DamageSettings, MeshComp->GetOwner());
			}
		}
	}
}
