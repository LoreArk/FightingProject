// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnPoint.h"
#include "FighterGameMode.h"
#include "EnemyBase.h"
#include "FighterGameMode_Arena.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API AFighterGameMode_Arena : public AFighterGameMode
{
	GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY()
    TSoftObjectPtr<UWorld> LevelToTransitionTo;

    UPROPERTY()
    float TimeBetweenRounds;

    UPROPERTY()
    int32 RoundCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AEnemyBase> ActorToSpawn;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    TArray<AEnemyBase*> SpawnedActors;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    TArray<AActor*> SpawnPoints;

    UFUNCTION()
    void SpawnEnemies(int32 EnemyCount);

    UFUNCTION()
    void OnEnemyDeath(AActor* Actor);

    UFUNCTION()
    void RoundEndTimer();

    UFUNCTION(BlueprintCallable)
    void StartRound();

    virtual void EnemyDrop(AActor* Actor) override;

    virtual void PlayerDeathEvent() override;

    virtual void StartDeathTimer(AActor* Actor) override;


    UPROPERTY()
    FTimerHandle RoundTimerHandle;

};
