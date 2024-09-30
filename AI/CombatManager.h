// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"

UCLASS()
class FIGHTINGPROJECT_API ACombatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatManager();


	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> AttackingActors;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> DeadEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAttackingActors;

	UPROPERTY()
	int32 MaxAttackToken;

	UPROPERTY(VisibleAnywhere)
	int32 AvailableAttackToken;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Death(AActor* Actor);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	bool CanAttack(AActor* Actor);

	UFUNCTION()
	void AddAttackingActor(AActor* Actor);

	UFUNCTION()
	void EnemyDeathHandler(AActor* Actor);

	UFUNCTION()
	void RemoveAttackingActor(AActor* Actor);
};
