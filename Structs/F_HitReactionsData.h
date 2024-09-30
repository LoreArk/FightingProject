// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "F_HitReactionSettings.h"
#include "F_HitReactionsData.generated.h"

USTRUCT(BlueprintType)
struct F_HitReactionsData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Data")
	TMap<UAnimMontage*, FHitReactionSettings> HitReactions;

};
