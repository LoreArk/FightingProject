// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "F_DamageSettings.h"
#include "Interface_Interactable.h"
#include "Interface_Carriable.h"
#include "Public/Component_ObjectPool.h"
#include "Interface_Damagable.h"
#include "Components/PrimitiveComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "PickableActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThrow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectSpawn, ACharacter*, OwnerChar);


UENUM(BlueprintType)
enum class EPickableType : uint8
{
	Box UMETA(DisplayName = "Box"),
	Club UMETA(DisplayName = "Club")
};


UCLASS()
class FIGHTINGPROJECT_API APickableActor : public AActor, public IInterface_Interactable, public IInterface_Carriable, public IInterface_Damagable
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//USceneComponent* Scene;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Carry Settings")
	FName CarrySocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX Settings")
	UComponent_ObjectPool* SpawnedPoolComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage Settings")
	FDamageSettings DamageSettings = FDamageSettings();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage Settings")
	float DamageRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UGeometryCollectionComponent* GeometryCollectionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* SpawnObjectReference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* LeftHandReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EPickableType ObjectType;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnThrow OnThrow;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPickUp OnPickUp;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnObjectSpawn OnObjectSpawn;

	UPROPERTY(BlueprintReadOnly, BlueprintReadOnly)
	bool thrown;

	UPROPERTY(BlueprintReadOnly, BlueprintReadOnly)
	bool bBroken;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Spawn Settings")
	bool bShouldSpawnItem;

	UPROPERTY(BlueprintReadOnly)
	AActor* ThrowrActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDontDestroyBeforePickUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 Durability;

	UPROPERTY()
	TArray<AActor*> DamagedActor;

	UPROPERTY()
	bool bHitTraceFlag;

	APickableActor();

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void HitTrace();

public:	
	
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation() override;

	virtual APickableActor* GetCarriable_Implementation() override;

	virtual void TakeDamage_Implementation(FDamageSettings DamageSettings, AActor* InstigatorActor) override;

	virtual bool CanThrow_Implementation() override;

	UFUNCTION()
	virtual void OnPickUpHandler();

	UFUNCTION()
	virtual void OnObjectSpawnHandler(ACharacter* OwnerChar);

	UFUNCTION()
	void OnThrowHandler();

	UFUNCTION(BlueprintNativeEvent)
	void OnBreakEvent();
	void OnBreakEvent_Implementation();

	UFUNCTION(BlueprintCallable)
	void Break();

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnImpactVFX(FTransform SpawnTransform, FHitResult HitResult);
	void SpawnImpactVFX_Implementation(FTransform SpawnTransform, FHitResult HitResult);


};
