// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_SetMovementMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface_CombatCharacter.h"

void UAnimNotifyState_SetMovementMode::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);


	AActor* OwnerActor = MeshComp->GetOwner();


	if (OwnerActor) 
	{
		if (OwnerActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
		{
			CachedCharacter = IInterface_CombatCharacter::Execute_GetCombatCharacter(OwnerActor);

			if (CachedCharacter)
			{
				CachedCharacter->GetCharacterMovement()->SetMovementMode(MovementMode);
			}
		}
	}

}

void UAnimNotifyState_SetMovementMode::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (CachedCharacter)
	{
		CachedCharacter->GetCharacterMovement()->SetMovementMode(EndMovementMode);
	}

}
