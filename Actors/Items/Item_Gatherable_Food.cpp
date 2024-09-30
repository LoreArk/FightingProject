// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Gatherable_Food.h"
#include "Interface_Player.h"

void AItem_Gatherable_Food::Interact_Implementation()
{
	if (InteractingActor == nullptr)
		return;

	if (InteractingActor->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
	{
		IInterface_Player::Execute_AddHealth(InteractingActor, HealthAmount);
	}


	Super::Interact_Implementation();

}
