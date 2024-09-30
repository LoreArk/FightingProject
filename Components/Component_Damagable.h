// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_DamageSettings.h"
#include "GameFramework/Actor.h"
#include  "F_HitReactionSettings.h"
#include "Components/ActorComponent.h"
#include "Component_Damagable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, Actor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGPROJECT_API UComponent_Damagable : public UActorComponent
{
	GENERATED_BODY()

public:	

	UComponent_Damagable();

	UPROPERTY( BlueprintAssignable, Category = "Events")
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStun OnStun;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStunEnd OnStunEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead;

	// Sets default values for this component's properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Health;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxStunValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float StunValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StunDecreaseRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StunIncreaseValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PoiseValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasPoise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReceiveStagger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsStaggered;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsStunned;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float StunDuration;

	FTimerHandle StunTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsInvincible;

	UPROPERTY()
	AActor* LastDamageDealer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Moveset Settings")
	UDataTable* HitReactionsDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitReactions")
	TMap<UAnimMontage*, FHitReactionSettings> WeakHitReactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitReactions")
	TMap<UAnimMontage*, FHitReactionSettings> MedHitReactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitReactions")
	TMap<UAnimMontage*, FHitReactionSettings> StunHitReactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitReactions")
	TMap<UAnimMontage*, FHitReactionSettings> LaunchHitReactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitReactions")
	TMap<UAnimMontage*, FHitReactionSettings> FallHitReactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitReactions")
	TMap<UAnimMontage*, FHitReactionSettings> LayingHitReactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitReactions")
	TMap<UAnimMontage*, FHitReactionSettings> JuggleHitReactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitReactions")
	TMap<UAnimMontage*, FHitReactionSettings> StaggerHitReactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DeathAnimations")
	TMap<UAnimMontage*, FHitReactionSettings> DeathMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DeathAnimations")
	TMap<UAnimMontage*, FHitReactionSettings> DeathLaunchMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DeathAnimations")
	TMap<UAnimMontage*, FHitReactionSettings> DeathFallMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DeathAnimations")
	TMap<UAnimMontage*, FHitReactionSettings> DeathJuggleMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StunMontageHitReaction")
	UAnimMontage* StunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFrozenState CurrentFrozenState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FDamageSettings LastReceivedDamageSettings;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	float GetHealth();

	UFUNCTION()
	float GetMaxHealth();


	UFUNCTION()
	bool TakeDamage(FDamageSettings DamageSettings, AActor* Instigator);

	UFUNCTION()
	void TakeFinisherDamage(FDamageSettings DamageSettings);

	UFUNCTION()
	EHitDirection GetHitDirection(AActor* Instigator);

	UFUNCTION()
	void ProcessDamageForHitReaction(FDamageSettings DamageSettings, AActor* DamageInstigator);

	UFUNCTION()
	void ProcessDeathHitReaction(FDamageSettings DamageSettings, AActor* DamageInstigator);

	UFUNCTION()

	UAnimMontage* FindHitReactionMontage(FDamageSettings DamageSettings, EHitType HitType, EHitDirection OtherPosition);

	UFUNCTION()

	UAnimMontage* FindDeathMontage(FDamageSettings DamageSettings, EHitType HitType, EHitDirection OtherPosition);

	UFUNCTION()
	EHitDirection GetHitReactionDirection(EHitDirection OtherPosition, EHitDirection AttackDirection);

	UFUNCTION()
	void ReceiveStun(float StunPower);

	UFUNCTION()
	void ResetStun();

	UFUNCTION()
	void SetHasPoise(bool HasPoise);


private:
	void DecreaseStun(float DeltaTime);
};
