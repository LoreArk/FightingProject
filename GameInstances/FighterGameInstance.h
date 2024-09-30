// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_PersistentData.h"
#include "FightingProject/FightingProjectCharacter.h"
#include "Engine/GameInstance.h"
#include "F_LevelData.h"
#include "FighterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API UFighterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHubStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGameOver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldSetPlayerTransform;

	UPROPERTY()
	bool bShouldLoadData;

	UPROPERTY()
	bool bPlayerInitialized;

	UPROPERTY()
	bool bLoadChallengeLevel;

	UPROPERTY()
	FTimerHandle LoadLevelTimerHandle;

	UPROPERTY(VisibleAnywhere)
	FPersistentData PersistentData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform PlayerPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 MoneyAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ELevelType CurrentLevelType;

	UFUNCTION(BlueprintCallable)
	void GameOver(TSoftObjectPtr<UWorld> LevelToLoad);

	UFUNCTION(BlueprintCallable)
	void LevelTransition(FName LevelName, AActor* Player, FLevelLoadData LevelLoadData);

	UFUNCTION(BlueprintCallable)
	void LoadChallengeLevel(FName LevelName, AActor* Player, FLevelLoadData LevelLoadData);

	UFUNCTION(BlueprintCallable)
	void ExtiChallengeLevel(FName LevelName, AActor* Player, TSoftObjectPtr<UWorld> LevelToLoad);

	UFUNCTION(BlueprintCallable)
	void GatherPlayerState(AActor* Player);


	UFUNCTION()
	void ClearTimers();

	UFUNCTION()
	void StartLoadLevelTimer(FName LevelName, AActor* Player, TSoftObjectPtr<UWorld> LevelToLoad);

	UFUNCTION()
	void OnLoadLevel(FName LevelName, AActor* Player, TSoftObjectPtr<UWorld> LevelToLoad);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLevelData> AvailableArenaStages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FLevelData> AvailableMissionsStages;

protected:


};
