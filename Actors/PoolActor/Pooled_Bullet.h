// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledActor.h"
#include "F_DamageSettings.h"
#include "Pooled_Bullet.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API APooled_Bullet : public APooledActor
{
	GENERATED_BODY()
	
	APooled_Bullet();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

public: 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet Settings")
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet Settings")
	float BulletTraceRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet Settings")
	FDamageSettings DamageSettings;

	UFUNCTION(BlueprintNativeEvent)
	void FXHandler(FHitResult HitResult);
	void FXHandler_Implementation(FHitResult HitResult);

	UPROPERTY()
	bool bIsActive;

	UPROPERTY()
	TArray<AActor*> DamagedActors;

	UPROPERTY()
	FVector TargetLocation;

	virtual void SetInUse_Implementation(bool InUse) override;

};
