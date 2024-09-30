// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.h"
#include "Interface_Damagable.h"
#include "Interface_Finishers.h"
#include "Interface_CombatCharacter.h"
#include "StatsHUD.h"
#include "Component_Finishers.h"
#include "Component_ObjectPool.h"
#include "MotionWarpingComponent.h"
#include "F_DamageSettings.h"
#include "CombatCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationDoDamage);

UCLASS()
class FIGHTINGPROJECT_API ACombatCharacter : public ACharacterBase, public IInterface_Damagable, public IInterface_CombatCharacter, public IInterface_Finishers
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACombatCharacter();

	UPROPERTY()
	FOnAnimationDoDamage OnAnimationDoDamage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<AActor*> LastDamagedActors;

	UPROPERTY()
	bool bShouldDamageDealer;

	UPROPERTY()
	FName FightStartTrace;

	UPROPERTY()
	FName FightEndTrace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	FDamageSettings CurrentDamageSettings;

	UPROPERTY()
	FDamageSettings ReceivedDamageSettings;

	UPROPERTY()
	float CurrentHitTraceRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class APickableActor* CarriedActor;

	virtual void ConsumeWeaponUse();

	UFUNCTION(BlueprintCallable)
	virtual void ExitGrab();

	UFUNCTION()
	virtual void CombatPauseGlobalTimeDilation();

	UFUNCTION()
	virtual void CombatResumeGlobalTimeDilation();

	UFUNCTION()
	virtual void OnStartAttack(float distance, AActor* AttackTarget);

public:	

	UStatsHUD* StatsHud;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UComponent_Damagable* DamagableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UComponent_Finishers* FinishersComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UComponent_ObjectPool* FXActorPoolComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY()
	float ChargedAttackPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<UAnimMontage*> DodgeMontages;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UComponent_Damagable* GetDamagableComponent_Implementation() override;

	virtual void PlayHitReaction_Implementation(UAnimMontage* TargetMontage, AActor* DamageInstigator) override;

	virtual void HitTraceStart_Implementation(FDamageSettings DamageSettings, F_HitTraceSettings HitTraceSettings) override;

	virtual void HitTraceTick_Implementation() override;

	virtual void HitTraceEnd_Implementation() override;

	virtual bool IsFinishable_Implementation() override;

	virtual void SetFinishableActor_Implementation(AActor* FinishableActor) override;

	virtual AActor* GetFinisherVictim_Implementation() override;

	virtual void RemoveFinishableActor_Implementation(AActor* Actor);

	virtual void ExitGrabPose_Implementation() override;

	virtual ACombatCharacter* GetCombatCharacter_Implementation() override;
};
