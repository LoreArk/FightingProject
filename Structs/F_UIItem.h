// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_UIItem.generated.h"

USTRUCT(BlueprintType)
struct F_UIItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Item Settings")
    FString ItemName;
};
