// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_ResourcesDrop.h"

UComponent_ResourcesDrop::UComponent_ResourcesDrop()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UComponent_ResourcesDrop::BeginPlay()
{
	Super::BeginPlay();



}


// Called every frame
void UComponent_ResourcesDrop::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

