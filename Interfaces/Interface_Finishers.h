// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Camera/CameraComponent.h"
#include "Interface_Finishers.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Finishers : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIGHTINGPROJECT_API IInterface_Finishers
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ExecuteFinisher(FVector WarpingLocation, FRotator WarpingRotation, UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ReceiveFinisher(AActor* ActorToFace, UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetFinishableActor(AActor* FinishableActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void RemoveFinishableActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AActor* GetFinisherVictim();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	UCameraComponent* GetFinisherCamera();

};
