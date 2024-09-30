// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "F_AttackSettings.h"
#include "SpecialMovesSelectorUI.h"
#include "Projectile.h"
#include "Component_SpecialMoves.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGPROJECT_API UComponent_SpecialMoves : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComponent_SpecialMoves();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 OrbsNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxOrbsNumber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<UAnimMontage*, FSpecialAttackSettings> SpecialMoves;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USpecialMovesSelectorUI* SpecialMovesUI;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* SpecialMovePose;

	UFUNCTION()
	void OpenMenu();

	UFUNCTION()
	void CloseMenu();

	UFUNCTION()
	void SpawnGun();

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(TSubclassOf<AProjectile> Projectile, USkeletalMeshComponent* SkeletalMesh, FName SocketName, FDamageSettings DamageSettings);

	UFUNCTION()
	bool HasAvailableMoves();

	UPROPERTY()
	bool bExecutingFinisher;
};
