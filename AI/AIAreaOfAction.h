// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIAreaOfAction.generated.h"

UCLASS()
class FIGHTINGPROJECT_API AAIAreaOfAction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIAreaOfAction();

	// Helper function to check if a point is inside the bounding box
	bool IsPointInsideArea(const FVector& Point) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* ActionArea;

};
