// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FightingProject/PickableActor.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

UCLASS()
class FIGHTINGPROJECT_API AWeapon : public APickableActor
{
	GENERATED_BODY()
	

public:

	AWeapon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FString WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UBoxComponent* InteractCollider;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack Data")
	UDataTable* AttackDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack Data")
	UDataTable* AI_AttackDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSubclassOf<UAnimInstance> AnimInstanceClass;

	virtual APickableActor* GetCarriable_Implementation() override;

	UFUNCTION()
	void Drop();

private: 
	virtual void OnPickUpHandler() override;
};
