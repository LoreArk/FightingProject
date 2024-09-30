// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIModule.h"
#include "BehaviorTree/BehaviorTree.h"
#include "FightingProject/FightingProjectCharacter.h"
#include "Interface_Enemy.h"
#include "CombatManager.h"
#include "E_AIStates.h"
#include "AIC_EnemyBase.generated.h"

/**
 * 
 */
class FOnAttackFinished;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackFinished, AAIC_EnemyBase*, Attacker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterAttackState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitAttackState);

UCLASS()
class FIGHTINGPROJECT_API AAIC_EnemyBase : public AAIController, public IInterface_Enemy
{
	GENERATED_BODY()

public:
	explicit AAIC_EnemyBase(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	AActor* PlayerRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* AttackTarget;

	FOnAttackFinished OnAttackFinishedDelegate;
	FOnEnterAttackState OnCombatEnter;
	FOnExitAttackState OnCombatExit;

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	ACombatManager* CombatManager;


	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	FVector StartPosition;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bReachPlayer;

	UPROPERTY()
	class AEnemyBase* EnemyBaseRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EAIStates AiState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

	class UAISenseConfig_Sight* SightConfig;

	void SetupPerceptionSystem();

	float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;

	virtual AActor* GetAttackTarget_Implementation() override;

	void Attack_Implementation() override;

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);


	UFUNCTION()
	void OnDamageReceived(AActor* DamageDealer);

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION(BlueprintCallable)
	void SetStateAsAttacking();

	UFUNCTION(BlueprintCallable)
	void SetStateAsFrozen();

	UFUNCTION(BlueprintCallable)
	void SetStateAsPassive();

	UFUNCTION()
	void SetStateAsFinisher();

	UFUNCTION()
	void SetStateAsDead();

	UFUNCTION()
	void SetStateAsStun();

	UFUNCTION(BlueprintCallable)
	void SetStateAsSurrender();

	bool IsTargetWithinAreaOfAction(AActor* Target) const;
};
