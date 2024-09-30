// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "F_PersistentData.h"
#include "F_AttackSettings.h"
#include "Component_Attacks.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGPROJECT_API UComponent_Attacks : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComponent_Attacks();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Slots")
	FAttackMovesetData AttackMovesetData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permanent Attacks")
	TMap<UAnimMontage*, FAttackSettings> LowAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen State Attacks")
	TMap<UAnimMontage*, FAttackSettings> FellFrozenAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen State Attacks")
	TMap<UAnimMontage*, FAttackSettings> JuggleFrozenAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Inventory")
	TMap<UAnimMontage*, FAttackSettings> AttacksPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Settings")
	int32 MaxComboAttacks;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
