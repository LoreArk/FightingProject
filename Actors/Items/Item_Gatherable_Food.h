// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Gatherable.h"
#include "Item_Gatherable_Food.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API AItem_Gatherable_Food : public AItem_Gatherable
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthAmount;

	virtual void Interact_Implementation() override;
};
