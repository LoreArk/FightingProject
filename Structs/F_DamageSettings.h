// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "E_DamageEnums.h"
#include "F_DamageSettings.generated.h"

USTRUCT(BlueprintType)
struct FDamageSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    int32 DamageAmount;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    int32 StunPower;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    EDamageType DamageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    EDamageResponse DamageResponse;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    EHitType HitType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    EHitDirection HitDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    bool bShouldDamageInvincible;

    UPROPERTY(BlueprintReadOnly, Category = "Damage Settings")
    float ChargedAttackPercentage;

    FDamageSettings()
        : DamageAmount(0)
        , DamageType(EDamageType::Physical)
        , DamageResponse(EDamageResponse::None)
        , HitType(EHitType::Mid)
        , HitDirection(EHitDirection::Front)
        , bShouldDamageInvincible(false)

    {}
};