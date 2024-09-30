// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAreaOfAction.h"
#include "Components/BoxComponent.h"

AAIAreaOfAction::AAIAreaOfAction()
{
    PrimaryActorTick.bCanEverTick = false;

	ActionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("ActionArea"));
	RootComponent = ActionArea;

    ActionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ActionArea->SetHiddenInGame(false); // Ensure it's visible in the game
    ActionArea->SetVisibility(true);
}

bool AAIAreaOfAction::IsPointInsideArea(const FVector& Point) const
{
    FVector BoxOrigin = ActionArea->GetComponentLocation();
    FVector BoxExtent = ActionArea->GetScaledBoxExtent();

    FBox BoundingBox(BoxOrigin - BoxExtent, BoxOrigin + BoxExtent);
    return BoundingBox.IsInside(Point);
}

void AAIAreaOfAction::BeginPlay()
{
	Super::BeginPlay();
	
}


