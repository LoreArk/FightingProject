// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Component_Damagable.h"
#include "F_HitTraceSettings.h"
#include "Interface_CombatCharacter.generated.h"


UINTERFACE(MinimalAPI)
class UInterface_CombatCharacter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIGHTINGPROJECT_API IInterface_CombatCharacter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool GetIsBlocking();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool GetIsFighting();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool GetIsStunned();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void HitTraceStart(FDamageSettings DamageSettings, F_HitTraceSettings HitTraceSettings);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void HitTraceTick();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void HitTraceEnd();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void PlayHitReaction(UAnimMontage* TargetMontage, AActor* DamageInstigator);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void PlayDeathMontage(UAnimMontage* TargetMontage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	UComponent_Damagable* GetDamagableComponent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SpawnWeaponSpawnableActor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void MovementComponentPush(FVector PushDirection, float PushForce);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool CanBeGrabbed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool IsFinishable();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ReceiveGrabAction(UAnimMontage* Montage, AActor* ActorToFace);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ReceiveGrabAttack(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ExitGrabPose();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void TryGrab();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	EFrozenState CanReceiveFrozenAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void HitWall(FHitResult HitResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	ACombatCharacter* GetCombatCharacter();
};
