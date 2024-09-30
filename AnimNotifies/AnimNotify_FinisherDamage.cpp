// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FinisherDamage.h"
#include "Interface_Finishers.h"
#include "Sound/SoundCue.h"
#include "FightingProject/Interface_Damagable.h"

void UAnimNotify_FinisherDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();
	
	if (Owner->GetClass()->ImplementsInterface(UInterface_Finishers::StaticClass()))
	{
		AActor* Victim = IInterface_Finishers::Execute_GetFinisherVictim(MeshComp->GetOwner());

		if (Victim)
		{
			if (Victim->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass()))
			{
				IInterface_Damagable::Execute_TakeDamage(Victim, DamageSettings, MeshComp->GetOwner());
			}
		}
	}
}
