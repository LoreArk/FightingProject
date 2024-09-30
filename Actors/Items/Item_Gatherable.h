// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Components/SphereComponent.h"
#include "FightingProject/Interface_Interactable.h"
#include "Interface_CombatCharacter.h"
#include "Item_Gatherable.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API AItem_Gatherable : public AItem, public IInterface_Interactable
{
	GENERATED_BODY()

public:

    AItem_Gatherable();

    virtual void Interact_Implementation() override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    // Sphere collision component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* SphereCollider;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ItemMesh;

    UPROPERTY()
    FVector TargetLocation;


    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tick Property")
    float ReachTargetLocationSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tick Property")
    float RotationSpeed;

    UPROPERTY()
    bool bShouldLerpToLocation;

    // Overlap begin function
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    // Overlap end function
    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UPROPERTY()
    AActor* InteractingActor;

    // Interact function
    
};
