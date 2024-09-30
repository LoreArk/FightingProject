// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_Player.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Player : public UInterface
{
	GENERATED_BODY()
};

class FIGHTINGPROJECT_API IInterface_Player
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void AddHealth(float HealthToAdd);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void AddOrbs(int32 OrbsToAdd);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void AddMaxOrbs(int32 OrbToAdd);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void AddMaxHealth(float HealthToAdd);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AActor* GetAttackTarget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetGatherableItem(AActor* Item);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AActor* GetGatherableItem();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void AttackAnticipationTrace();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AFightingProjectCharacter* GetPlayerCharacter();
};
