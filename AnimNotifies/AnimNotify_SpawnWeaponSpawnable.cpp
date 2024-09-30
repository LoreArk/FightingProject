// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnWeaponSpawnable.h"
#include "Interface_CombatCharacter.h"

void UAnimNotify_SpawnWeaponSpawnable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

	if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass())) 
	{
		IInterface_CombatCharacter::Execute_SpawnWeaponSpawnableActor(MeshComp->GetOwner());
	}
}
