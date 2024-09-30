// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "F_AttackSettings.h"
#include "Component_Finishers.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGPROJECT_API UComponent_Finishers : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComponent_Finishers();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FFinisherSettings> Finishers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* FinisherRecovery;

	UPROPERTY()
	bool bFinishable;

	UPROPERTY()
	AActor* Attacker;

	UPROPERTY()
	AActor* Victim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AActor*> FinishableActors;

	UFUNCTION()
	AActor* FinisherVulnerabilityTrace();

	UFUNCTION()
	void HandleFinisherRequest(AActor* Target);

	UFUNCTION()
	AActor* GetBestFinisherActor();

	UFUNCTION()
	void EndVulnerability();

};
