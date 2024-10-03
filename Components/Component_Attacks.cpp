// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_Attacks.h"

UComponent_Attacks::UComponent_Attacks()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UComponent_Attacks::BeginPlay()
{
	Super::BeginPlay();

}


void UComponent_Attacks::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

