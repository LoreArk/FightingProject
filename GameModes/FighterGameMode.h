// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "CombatManager.h"
#include "GameFramework/GameModeBase.h"
#include "FighterGameInstance.h"
#include "Interface_GameMode.h"
#include "FighterGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoneyDrop, int32, Amount, bool, subtract);

UCLASS()
class FIGHTINGPROJECT_API AFighterGameMode : public AGameModeBase, public IInterface_GameMode
{
    GENERATED_BODY()

public:

    AFighterGameMode();

    virtual void BeginPlay() override;

    virtual AFighterGameMode* GetFighterGameMode_Implementation() override;



    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TSoftObjectPtr<UWorld> GameOverLevelToLoad;

    UFUNCTION(BlueprintNativeEvent)
    void PauseGame();
    void PauseGame_Implementation();

    UFUNCTION(BlueprintCallable)
    void UnpauseGame();

    UPROPERTY()
    AActor* PlayerActor;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    ACombatManager* CombatManager;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    int32 MaxAttackingActors = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Item Settings")
    TMap<TSubclassOf<AItem>, float> SpawnableItem;

    UPROPERTY()
    float MoneyDropMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Item Settings")
    float NothingSpawnProbability;


    UFUNCTION()
    void TrySpawnItem(FVector SpawnLocation);

    UFUNCTION(BlueprintCallable)
    void SpawnItem(FVector SpawnLocation, TSubclassOf<AItem> ItemToSpawn);

    UFUNCTION()
    virtual void EnemyDrop(AActor* Actor);

    UFUNCTION(BlueprintCallable)
    void AddMoney(int32 Amount);

    UFUNCTION(BlueprintCallable)
    void SubtractMoney(int32 Amount);

    UFUNCTION(BlueprintCallable)
    virtual void PlayerDeathEvent();

    UFUNCTION(BlueprintCallable)
    virtual void StartDeathTimer(AActor* Actor);

    UFUNCTION()
    void ClearAllTimers();

    UPROPERTY()
    FTimerHandle PlayerDeathTimerHandle;

    UPROPERTY(BlueprintAssignable)
    FOnMoneyDrop OnMoneyDrop;


protected:

    UFighterGameInstance* GameInstance;
};
