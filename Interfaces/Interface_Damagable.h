// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "F_DamageSettings.h"
#include "Interface_Damagable.generated.h"


UINTERFACE(MinimalAPI)
class UInterface_Damagable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIGHTINGPROJECT_API IInterface_Damagable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void TakeDamage(FDamageSettings DamageSettings, AActor* Instigator);
};
