// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatManager.h"
#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "FighterGameMode.h"
#include "Interface_GameMode.h"
#include "Interface_Enemy.h"

// Sets default values
ACombatManager::ACombatManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxAttackToken = 3;
	AvailableAttackToken = MaxAttackToken;

	MaxAttackingActors = 1;
}

// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	

	//GameMode = Cast<AFighterGameMode>(GameModeBase);

}

void ACombatManager::Death(AActor* Actor)
{
	
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ACombatManager::CanAttack(AActor* Actor)
{
	if (AttackingActors.Num() < MaxAttackingActors || AttackingActors.Contains(Actor))
	{
		return true;
	}

	return false;
}

void ACombatManager::AddAttackingActor(AActor* Actor)
{
	AttackingActors.AddUnique(Actor);
	UComponent_Damagable* Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(Actor);
	Damagable->OnDeath.AddDynamic(this, &ACombatManager::EnemyDeathHandler);
}

void ACombatManager::EnemyDeathHandler(AActor* Actor)
{
	RemoveAttackingActor(Actor);

	if (!DeadEnemies.Contains(Actor))
	{
		DeadEnemies.Add(Actor);
		AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
		AFighterGameMode* FighterGameMode = IInterface_GameMode::Execute_GetFighterGameMode(GameModeBase);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "ENEMY DEATH HANDLER");

		FighterGameMode->EnemyDrop(Actor);
	}
}

void ACombatManager::RemoveAttackingActor(AActor* Actor)
{
	if (AttackingActors.Contains(Actor))
	{
		AttackingActors.Remove(Actor);
	}
}



