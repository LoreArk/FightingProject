// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Level.h"
#include "F_LevelData.generated.h"


UENUM(BlueprintType)
enum class ELevelType : uint8
{
    Hub UMETA(DisplayName = "Hub"),
    Mission UMETA(DisplayName = "Mission"),
    Challenge UMETA(DisplayName = "Challenge")
};

USTRUCT(BlueprintType)
struct FLevelUIData
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    FText LevelName;

    // Level description
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    FText LevelDescription;

    // Level objective
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    FText LevelObjective;

    // Level image
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    UTexture2D* LevelImage;

};

USTRUCT(BlueprintType)
struct FLevelLoadData
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Setting")
    ELevelType LevelType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Setting")
    TSoftObjectPtr<UWorld> LevelAsset;
};


USTRUCT(BlueprintType)
struct FLevelData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Setting")
    FLevelLoadData LevelLoadData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    FLevelUIData LevelUIData;
};


USTRUCT(BlueprintType)
struct  FLevelDataTable : public FTableRowBase
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Data")
    TArray<FLevelData> LevelsData;
};