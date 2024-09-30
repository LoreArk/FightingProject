// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Component_Damagable.h"
#include "FighterGameMode.h"
#include "Interface_CombatCharacter.h"



void UFighterGameInstance::GameOver(TSoftObjectPtr<UWorld> LevelToLoad)
{
	ClearTimers();

	MoneyAmount = MoneyAmount / 2;
	bShouldLoadData = false;		
	GatherPlayerState(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	bGameOver = true;

	CurrentLevelType = ELevelType::Hub;
	StartLoadLevelTimer("None", nullptr, LevelToLoad);
}

void UFighterGameInstance::LevelTransition(FName LevelName, AActor* Player, FLevelLoadData LevelLoadData)
{
	ClearTimers();

	if (!LevelName.IsNone())
	{
		bShouldLoadData = true;
		GatherPlayerState(Player);
		UE_LOG(LogTemp, Error, TEXT("GAME INSTANCE OPEN LEVEL"));

		CurrentLevelType = LevelLoadData.LevelType;
		StartLoadLevelTimer(LevelName, Player, LevelLoadData.LevelAsset);
	}
}

//EXIT TEMPORARELY FROM LEVEL KEEPING PLAYER POSITION
void UFighterGameInstance::LoadChallengeLevel(FName LevelName, AActor* Player, FLevelLoadData LevelLoadData)
{
	ClearTimers();

	bShouldLoadData = false;
	bLoadChallengeLevel = true;
	PlayerPosition = Player->GetActorTransform();
	GatherPlayerState(Player);
	UE_LOG(LogTemp, Error, TEXT("GAME INSTANCE OPEN LEVEL"));

	CurrentLevelType = LevelLoadData.LevelType;
	StartLoadLevelTimer(LevelName, Player, LevelLoadData.LevelAsset);
	
}

void UFighterGameInstance::ExtiChallengeLevel(FName LevelName, AActor* Player, TSoftObjectPtr<UWorld> LevelToLoad)
{
	ClearTimers();

	bShouldLoadData = true;
	bShouldSetPlayerTransform = true;
	UE_LOG(LogTemp, Error, TEXT("GAME INSTANCE OPEN LEVEL"));

	CurrentLevelType = ELevelType::Hub;
	StartLoadLevelTimer(LevelName, Player, LevelToLoad);
}

void UFighterGameInstance::GatherPlayerState(AActor* Player)
{
	AFightingProjectCharacter* PlayerClass = Cast<AFightingProjectCharacter>(Player);
	UComponent_Damagable* Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(Player);

	PersistentData.Health = Damagable->Health;
	PersistentData.MaxHealth = Damagable->MaxHealth;
	PersistentData.Orbs = PlayerClass->SpecialMovesComponent->OrbsNumber;
	PersistentData.MaxOrbs = PlayerClass->SpecialMovesComponent->MaxOrbsNumber;

	PersistentData.EquippedMoveset = PlayerClass->AttacksComponent->AttackMovesetData;
	PersistentData.MaxComboAttacks = PlayerClass->AttacksComponent->MaxComboAttacks;
	/*
	PersistentData.EquippedMoveset.AttackSlot_1 = PlayerClass->AttacksComponent->AttackSlot_1;
	PersistentData.EquippedMoveset.AttackSlot_1_D = PlayerClass->AttacksComponent->AttackSlot_1_D;
	PersistentData.EquippedMoveset.AttackSlot_2 = PlayerClass->AttacksComponent->AttackSlot_2;
	PersistentData.EquippedMoveset.AttackSlot_2_D = PlayerClass->AttacksComponent->AttackSlot_2_D;
	PersistentData.EquippedMoveset.AttackSlot_3 = PlayerClass->AttacksComponent->AttackSlot_3;
	PersistentData.EquippedMoveset.AttackSlot_3_D = PlayerClass->AttacksComponent->AttackSlot_3_D;*/
}


void UFighterGameInstance::ClearTimers()
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	IInterface_GameMode::Execute_GetFighterGameMode(GameMode)->ClearAllTimers();
}

void UFighterGameInstance::StartLoadLevelTimer(FName LevelName, AActor* Player, TSoftObjectPtr<UWorld> LevelToLoad)
{
	float Delay = 1.0f;

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, FName("OnLoadLevel"), LevelName, Player, LevelToLoad);

	GetWorld()->GetTimerManager().SetTimer(LoadLevelTimerHandle, Delegate, Delay, false);
}

void UFighterGameInstance::OnLoadLevel(FName LevelName, AActor* Player, TSoftObjectPtr<UWorld> LevelToLoad)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Orange, " TIMER LOAD LEVEL");

	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LevelToLoad);

}
