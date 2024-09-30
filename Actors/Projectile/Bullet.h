// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Bullet.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API ABullet : public AProjectile
{
	GENERATED_BODY()


public:

    virtual void Launch(FVector TargetLocation) override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	bool bActivated;

protected:
	virtual void BeginPlay() override;
};
