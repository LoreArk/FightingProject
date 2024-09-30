// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PooledActor.h"
#include "Component_ObjectPool.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGPROJECT_API UComponent_ObjectPool : public UActorComponent
{
	GENERATED_BODY()

public:	

	UComponent_ObjectPool();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX to pool settings")
	TSubclassOf<APooledActor> AttackHitActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX to pool settings")
	TSubclassOf<APooledActor> AttackHitActor_2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX to pool settings")
	int32 AttackHitPoolSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX to pool settings")
	int32 AttackHitPoolSize_2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> AttackHitPool;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> AttackHitPool_2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectiles to pool settings")
	TSubclassOf<APooledActor> BulletActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectiles to pool settings")
	int32 BulletPoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> BulletPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectiles to pool settings")
	TSubclassOf<APooledActor> GranadeActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectiles to pool settings")
	int32 GranadePoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> GranadePool;

	UFUNCTION(BlueprintCallable)
	APooledActor* SpawnActorFromPool(FTransform SpawnTransform, TArray<APooledActor*> Pool);


protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void InitializePools();

	UFUNCTION()
	APooledActor* FindFirstAvailableActorInPool(TArray<APooledActor*> ActorPool);


public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
