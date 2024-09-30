// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_AttackSettings.h"

class FIGHTINGPROJECT_API StaticFunctions
{
public:
    // Add static functions here
    static FHitResult SphereCollisionTraceObject(UObject* WorldContextObject, FVector Start, FVector End, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes, TArray<AActor*> ActorsToIgnore, float Radius);
    static FHitResult SphereCollisionTraceChannel(UObject* WorldContextObject, FVector Start, FVector End, ETraceTypeQuery TraceChannel, TArray<AActor*> ActorsToIgnore, float Radius);
    static TArray<FHitResult> SphereCollisionMultiTraceByChannel(UObject* WorldContextObject, FVector Start, FVector End, ETraceTypeQuery TraceChannel, TArray<AActor*> ActorsToIgnore, float Radius);
    static void CalculateHitReaction(AActor* Actor);
    static TPair<FName, FName> GetSocketNameFromFightTraceEnum(EFightTrace EnumValue);
    static float GetAngleFromActorAToB(AActor* ActorA, AActor* ActorB);
    static float GetAngleFromActorWithSideVector(AActor* ActorA, AActor* ActorB);
    static FVector GetLocationAtDistanceXFromActorB(AActor* ActorA, AActor* ActorB, float Distance);
    static FRotator GetRotationForFacingDirection(AActor* Actor, AActor* ActorToFace);
    static FName GetFXSocketNameFromFightTrace(EFightTrace FightTrace);
    static float GetHorizontalDistanceBetweenActors(AActor* ActorA, AActor* ActorB);
};
