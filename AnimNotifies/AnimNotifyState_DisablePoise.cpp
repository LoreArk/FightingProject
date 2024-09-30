// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_DisablePoise.h"
#include "Interface_CombatCharacter.h"
#include "Component_Damagable.h"

void UAnimNotifyState_DisablePoise::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	if (MeshComp->GetOwner())
	{
		if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
		{
			DamagableComponent = IInterface_CombatCharacter::Execute_GetDamagableComponent(MeshComp->GetOwner());
			if (DamagableComponent)
			{
				DamagableComponent->SetHasPoise(false);
			}
		}
	}
}

void UAnimNotifyState_DisablePoise::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (DamagableComponent)
	{
		DamagableComponent->SetHasPoise(false);
	}
}

void UAnimNotifyState_DisablePoise::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (DamagableComponent)
	{
		DamagableComponent->SetHasPoise(true);
	}

}
