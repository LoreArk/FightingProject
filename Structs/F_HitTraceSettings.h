// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_AttackSettings.h"
#include "F_HitTraceSettings.generated.h"

USTRUCT(BlueprintType)
struct F_HitTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	EFightTrace FightTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	float HitTraceRadius;

public:
};
