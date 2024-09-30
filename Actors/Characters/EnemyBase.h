// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component_Damagable.h"
#include "GameFramework/Character.h"
#include "CombatCharacter.h"
#include "FightingProject/Interface_Damagable.h"
#include "F_HitReactionSettings.h"
#include "AIModule.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIC_EnemyBase.h"
#include "F_HitTraceSettings.h"
#include "FightingProject/PickableActor.h"
#include "F_AIAttacks.h"
#include "TimerManager.h"
#include "Interface_Finishers.h"
#include "AIAreaOfAction.h"
#include "Component_ResourcesDrop.h"
#include "Component_ObjectPool.h"
#include "Component_Finishers.h"
#include "StatsHUD.h"
#include "Interface_CombatCharacter.h"
#include "EnemyBase.generated.h"


UCLASS()
class FIGHTINGPROJECT_API AEnemyBase : public ACombatCharacter, public IInterface_Enemy, public IInterface_Interactable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	AAIC_EnemyBase* AIC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UAnimMontage* CurrentHitReaction;

	UPROPERTY()
	ACombatManager* CombatManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UComponent_ResourcesDrop* ResourcesComponent;

	UPROPERTY()
	float CurrentHitRadius;

	UBehaviorTree* GetBehviorTree();

	UPROPERTY(EditDefaultsOnly)
	UDataTable* AttackUnarmedMovesetData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<UAnimMontage*, FAIAttacks> Attacks;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<UAnimMontage*, FAIAttacks> WeaponAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APickableActor> WeaponToEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<UAnimMontage*, FAIAttacks> CounterAttacks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* GetUpMontage;

	UPROPERTY()
	int ComboCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<UAnimMontage*, FAIAttacks> CurrentAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* FinishersCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY()
	bool bCooling;

	UPROPERTY(BlueprintReadOnly)
	bool bIsBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior Settings", meta = (AllowPrivateAccess = "true"))
	bool bShouldTryDodge;

	UPROPERTY()
	UAnimMontage* CurrentDodgeMontage;

	UPROPERTY(VisibleAnywhere)
	bool bAttackSelectionTriggerFlag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bRangedCombat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AAIAreaOfAction* AreaOfAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> DefaultAnimClassLayer;

	UPROPERTY()
	bool bJuggleApexFlag;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* Tree;

	UPROPERTY()
	FTimerHandle CooldownTimerHandle;

	UPROPERTY()
	FTimerHandle AttackTimerHandle;

	UPROPERTY()
	FTimerHandle BlockTimerHandle;

	UPROPERTY()
	int32 HitTakenCount;

	UPROPERTY()
	int32 BlockedHitCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	int32 HitForBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	int32 MinHitForBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	int32 MaxHitForBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	int32 MinHitForCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	int32 MaxHitForCounter;

	UPROPERTY()
	int32 BlockedHitsForCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CurrentGrabPose;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Settings", meta = (AllowPrivateAccess = "true"))
	float HitMontagesRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Settings", meta = (AllowPrivateAccess = "true"))
	float AttackMontagesRate;

	UPROPERTY()
	FName FightTraceStart;

	UPROPERTY()
	FName FightTraceEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UWidgetComponent* StatsWidgetComponent;

	UPROPERTY()
	AActor* FinisherActor;

	virtual FRotator GetBaseAimRotation() const override;


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsAimed;
	// Called every frame

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void TakeDamage_Implementation(FDamageSettings DamageSettings, AActor* InstigatorActor) override;

	virtual void SetIsTargeted_Implementation(bool bIsTargeted) override;

	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;

	virtual AEnemyBase* GetOwnerClass_Implementation() override;

	virtual bool GetIsBlocking_Implementation() override;

	virtual bool GetIsStunned_Implementation() override;

	virtual void PlayHitReaction_Implementation(UAnimMontage* TargetMontage, AActor* DamageInstigator) override;

	virtual void PlayDeathMontage_Implementation(UAnimMontage* TargetMontage) override;

	virtual void SpawnWeaponSpawnableActor_Implementation() override;

	void Attack_Implementation() override;

	virtual AActor* GetAttackTarget_Implementation() override;

	void ReceiveFinisher_Implementation(AActor* ActorToFace, UAnimMontage* Montage) override;

	virtual UCameraComponent* GetFinisherCamera_Implementation() override;

	virtual void MovementComponentPush_Implementation(FVector PushDirection, float Strenght) override;

	virtual void Interact_Implementation() override;

	virtual bool CanBeGrabbed_Implementation() override;

	virtual void ReceiveGrabAction_Implementation(UAnimMontage* Montage, AActor* ActorToFace) override;

	virtual void ReceiveGrabAttack_Implementation(UAnimMontage* Montage);

	virtual void HitWall_Implementation(FHitResult HitResult) override;

	virtual void ExitGrab() override;

	virtual EFrozenState CanReceiveFrozenAttack_Implementation() override;

	UFUNCTION()
	void UpdateReadyAttacks();

	UFUNCTION(BlueprintCallable)
	void OnReceiveFinisherEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	void DefaultAttack(AActor* AttackTarget);
	void DefaultAttack_Implementation(AActor* AttackTarget);

	UFUNCTION(BlueprintNativeEvent)
	void CounterAttack();
	void CounterAttack_Implementation();

	UFUNCTION(BlueprintCallable)
	void EndHitReaction(UAnimMontage* HitReactionMontage);

	UFUNCTION()
	void AttackCooldownTimer(UAnimMontage* Attack);

	UFUNCTION()
	void AggroCooldownTimer();

	UFUNCTION()
	void StartCooldownTimer(UAnimMontage* Attack);

	UFUNCTION()
	void StartBlockTimer(int32 CurrentBlockedHit);

	UFUNCTION()
	void BlockTimer(int32 CurrentBlockedHit);
	
	UFUNCTION(BlueprintNativeEvent)
	void AttackMontagePlay(const TMap<UAnimMontage*, FAIAttacks>& ComboAttacks, int32 AttacksCount);
	void AttackMontagePlay_Implementation(const TMap<UAnimMontage*, FAIAttacks>& ComboAttacks, int32 AttacksCount);

	UFUNCTION(BlueprintNativeEvent)
	void PlayMontage(UAnimMontage* MontageToPlay);
	void PlayMontage_Implementation(UAnimMontage* MontageToPlay);

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UFUNCTION(BlueprintNativeEvent)
	void BlockStance();
	void BlockStance_Implementation();

	UFUNCTION()
	void OnStunEvent();

	UFUNCTION()
	void EndStunState();

	UFUNCTION(BlueprintCallable)
	void ShutDownAfterDeathMontage();

	UPROPERTY(VisibleAnywhere)
	TMap<UAnimMontage*, FAIAttacks> AvailableAttacks;

	UPROPERTY(VisibleAnywhere)
	TMap<UAnimMontage*, FAIAttacks> ReadyAttacks;

	UFUNCTION()
	bool GetTokenForAvailableAttack(int32 TokenNeeded);

	UFUNCTION()
	void InvalidateAllTimers();

	UFUNCTION()
	void DropWeapon();

	UFUNCTION()
	bool ShouldStopMontageForHitReaction();

	UFUNCTION(BlueprintNativeEvent)
	void BlockAttackFX(FDamageSettings DamageSettings);
	void BlockAttackFX_Implementation(FDamageSettings DamageSettings);
	
	UPROPERTY()
	int32 TokenKept;


private:

	UFUNCTION()
	void OnDeathEvent(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void OnFinisherVictimEnd();

	UFUNCTION()
	bool GetCanPlayAttackMontage();

	UFUNCTION()
	void PlayGetUpMontage();

	UFUNCTION()
	int GetDodgeMontageIndex();

	UFUNCTION()
	bool PerformSphereTraceWithDirection(FVector Direction);

	UFUNCTION()
	void StartAttacksUpdate();
	UFUNCTION()
	void StopAttacksUpdate();

	UPROPERTY()
	FTimerHandle UpdateAttackTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages Priority", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HighPriorityAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages Priority", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HighPriorityStunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages Priority", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;
};
