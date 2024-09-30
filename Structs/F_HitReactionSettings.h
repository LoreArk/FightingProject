// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "E_DamageEnums.h"
#include "F_DamageSettings.h"
#include "F_AttackSettings.h"
#include "F_HitReactionSettings.generated.h"

USTRUCT(BlueprintType)
struct FHitReactionSettings
{
    GENERATED_BODY()


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    EDamageType DamageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    EHitType HitType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    EHitDirection HitDirection;



    FHitReactionSettings()
        : DamageType(EDamageType::Physical)
        , HitType(EHitType::Mid)
        , HitDirection(EHitDirection::Front)

    {}
};