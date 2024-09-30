// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_AttackSettings.h"
#include "F_PersistentData.generated.h"


USTRUCT(BlueprintType)
struct FAttackMovesetData
{

    GENERATED_BODY()


    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<UAnimMontage*, FAttackSettings> AttackSlot_1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<UAnimMontage*, FAttackSettings> AttackSlot_1_D;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<UAnimMontage*, FAttackSettings> AttackSlot_2;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<UAnimMontage*, FAttackSettings> AttackSlot_2_D;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<UAnimMontage*, FAttackSettings> AttackSlot_3;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<UAnimMontage*, FAttackSettings> AttackSlot_3_D;



};

USTRUCT(BlueprintType)
struct FPersistentData
{
    GENERATED_BODY()

    UPROPERTY()
    float MaxHealth;

    UPROPERTY()
    float Health;

    UPROPERTY()
    int32 MaxOrbs;

    UPROPERTY()
    int32 Orbs;

    UPROPERTY()
    FAttackMovesetData EquippedMoveset;

    UPROPERTY()
    TMap<UAnimMontage*, FAttackSettings> AttacksPool;

    UPROPERTY()
    TMap<UAnimMontage*, FAttackSettings> ShoppableAttacks;

    UPROPERTY()
    int32 MaxComboAttacks;
};

