// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_GameMode.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_GameMode : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIGHTINGPROJECT_API IInterface_GameMode
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AFighterGameMode* GetFighterGameMode();
};
