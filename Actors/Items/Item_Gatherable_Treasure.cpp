// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Gatherable_Treasure.h"
#include "Interface_Player.h"
#include "Kismet/GameplayStatics.h"
#include "FighterGameMode.h"

void AItem_Gatherable_Treasure::Interact_Implementation()
{
	if (InteractingActor == nullptr)
		return;

	if (InteractingActor->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
	{
		AFighterGameMode* GameMode = Cast<AFighterGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		GameMode->AddMoney(MoneyValue);
	}

	Super::Interact_Implementation();
}
