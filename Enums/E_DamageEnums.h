// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "E_DamageEnums.generated.h"


UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Physical UMETA(DisplayName = "Physical"),
    Bullet UMETA(DisplayName = "Bullet"),
    Fire UMETA(DisplayName = "Fire"),
    Strike UMETA(DisplayName = "Strike"),
    Slash UMETA(DisplayName = "Slash")
};

UENUM(BlueprintType)
enum class EDamageResponse : uint8
{
    None UMETA(DisplayName = "None"),
    Weak UMETA(DisplayName = "Weak"),
    Med UMETA(DisplayName = "Med"),
    Stun UMETA(DisplayName = "Stun"),
    Stagger UMETA(DisplayName = "Stagger"),
    Launch UMETA(DisplayName = "Launch"),
    Fall UMETA(DisplayName = "Fall"),
    Juggle UMETA(DisplayName = "Juggle")
};

UENUM(BlueprintType)
enum class EHitType : uint8
{
    Low UMETA(DisplayName = "Low"),
    Mid UMETA(DisplayName = "Mid"),
    High UMETA(DisplayName = "High"),
    Top UMETA(DisplayName = "Top"),
    Upper UMETA(DisplayName = "Upper"),
};

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
    Front UMETA(DisplayName = "Front"),
    Back UMETA(DisplayName = "Back"),
    Left UMETA(DisplayName = "Left"),
    Right UMETA(DisplayName = "Right")
};


UENUM(BlueprintType)
enum class EMovementSpeed : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    Walk UMETA(DisplayName = "Walk"),
    Jog UMETA(DisplayName = "Jog"),
    Sprint UMETA(DisplayName = "Sprint")
};

UENUM(BlueprintType)
enum class EAI_States : uint8
{
    Passive UMETA(DisplayName = "Passive"),
    Combat UMETA(DisplayName = "Combat"),
    Frozen UMETA(DisplayName = "Frozen"),
};

UENUM(BlueprintType)
enum class EFrozenState : uint8
{
    None UMETA(DisplayName = "None"),
    Fell UMETA(DisplayName = "Fell"),
    Flying UMETA(DisplayName = "Flying"),
    Launched UMETA(DisplayName = "Launched"),
    Juggling UMETA(DisplayName = "Juggling")
};