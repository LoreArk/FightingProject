// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "E_DamageEnums.h"
#include "Interface_Enemy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Enemy : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIGHTINGPROJECT_API IInterface_Enemy
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	float SetMovementSpeed(EMovementSpeed Speed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AActor* GetAttackTarget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void Attack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void CombatSetAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetIsTargeted(bool bIsTargeted);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AEnemyBase* GetOwnerClass();

};
