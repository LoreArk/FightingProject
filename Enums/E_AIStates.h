// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAIStates : uint8
{
	Passive UMETA(DisplayName = "Passive"),
	Attack UMETA(DisplayName = "Attack"),
	Wait UMETA(DisplayName = "Wait"),
	Frozen UMETA(DisplayName = "Frozen"),
	Finisher UMETA(DisplayName = "Finisher"),
	Dead UMETA(DisplayName = "Dead"),
	Stun UMETA(DisplayName = "Stun"),
	Surrender UMETA(DisplayName = "Surrender")
};
