// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Granade.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API AGranade : public AProjectile
{
	GENERATED_BODY()


public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
	float ExplosionRadius;

	UPROPERTY()
	bool bActivated;

	virtual void Launch(FVector TargetLocation) override;

	virtual void Tick(float DeltaTime) override;

	void Activate();


protected:
	virtual void BeginPlay() override;
};
