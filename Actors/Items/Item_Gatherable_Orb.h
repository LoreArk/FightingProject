// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Gatherable.h"
#include "Item_Gatherable_Orb.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API AItem_Gatherable_Orb : public AItem_Gatherable
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OrbsAmount;

	virtual void Interact_Implementation() override;

};
