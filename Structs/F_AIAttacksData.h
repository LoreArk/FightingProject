// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "F_AIAttacks.h"
#include "F_AIAttacksData.generated.h"


USTRUCT(BlueprintType)
struct  F_AIAttacksData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Data")
	TMap<UAnimMontage*, FAIAttacks> AttackMapAction;
};
