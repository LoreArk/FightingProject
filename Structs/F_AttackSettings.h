// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "F_DamageSettings.h"
#include "E_DamageEnums.h"
#include "Projectile.h"
#include "F_DamageSettings.h"
#include "CameraAnimationSequence.h"
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>
#include <MovieSceneSequencePlayer.h>
#include "F_AttackSettings.generated.h"


UENUM(BlueprintType)
enum class EFightTrace : uint8
{
    RightHand UMETA(DisplayName = "RightHand"),
    LeftHand UMETA(DisplayName = "LeftHand"),
    RightFoot UMETA(DisplayName = "RightFoot"),
    LeftFoot UMETA(DisplayName = "LeftFoot"),
    RightKnee UMETA(DisplayName = "RightKnee"),
    LeftKnee UMETA(DisplayName = "LeftKnee"),
    RightElbow UMETA(DisplayName = "RightElbow"),
    LeftElbow UMETA(DisplayName = "LeftElbow"),
    Body UMETA(DisplayName = "Body"),
    Club UMETA(DisplayName = "Club"),
    Katana UMETA(DisplayName = "Katana")
};


USTRUCT(BlueprintType)
struct FSpecialAttackSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Moves Settings")
    int32 OrbsCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    FDamageSettings DamageSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Settings")
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Animation")
    TSoftObjectPtr<UCameraAnimationSequence> CameraAnimation;
    
    FSpecialAttackSettings()
        : OrbsCost(0)
    {
    }
};

USTRUCT(BlueprintType)
struct FFinisherSettings
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Finisher Settings")
    UAnimMontage* FinisherAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Finisher Settings")
    UAnimMontage* FinisherVictim;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Finisher Settings")
    float Distance;


};

USTRUCT(BlueprintType)
struct FUIAttackData
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack UI Settings")
    FName AttackName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack UI Settings")
    FText AttackProperty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack UI Settings")
    FText AttackDamage;
};

USTRUCT(BlueprintType)
struct FAttackSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coombo Settings")
    float Distance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coombo Settings")
    FString SortString;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
    EFightTrace FightTrace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
    FDamageSettings DamageSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack UI Settings")
    FUIAttackData UIData;

public:

    FAttackSettings()
        : SortString(""),
          FightTrace(EFightTrace::RightHand),
          DamageSettings(FDamageSettings()),
          UIData(FUIAttackData())
    {}
};
