// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "E_DamageEnums.h"
#include "F_DamageSettings.h"
#include "F_AttackSettings.h"
#include "F_AIAttacks.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FAIAttacks
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Settings")
    FString SortString;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
    EFightTrace FightTrace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    FDamageSettings DamageSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
    float MinDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
    float MaxDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
    float CooldownTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
    float CooldownTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
    float Weight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
    int32 TokenCost;

public:
	
    FAIAttacks()
        : SortString(""),
        FightTrace(EFightTrace::RightHand),
        DamageSettings(FDamageSettings())
    {}

};
