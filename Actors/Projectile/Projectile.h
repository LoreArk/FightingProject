// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PooledActor.h"
#include "F_DamageSettings.h"
#include "Projectile.generated.h"

UCLASS()
class FIGHTINGPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage Settings")
	FDamageSettings DamageSettings = FDamageSettings();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage Settings")
	float DamageRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
	float Speed;

	UPROPERTY()
	AActor* TargetActor;
	
	virtual void Launch(FVector TargetLocation);

	UFUNCTION(BlueprintNativeEvent)
	void DestroyActor();
	virtual void DestroyActor_Implementation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<AActor*> DamagedActors;

	void AdjustVelocity();

	UPROPERTY(BlueprintReadOnly, Category = "Projectile")
	FVector LaunchDirection;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
