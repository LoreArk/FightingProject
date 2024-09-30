// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterGameMode_Arena.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "FighterGameInstance.h"
#include "SpawnPoint.h"

void AFighterGameMode_Arena::BeginPlay()
{
    Super::BeginPlay();

	RoundCount = 0;
    TimeBetweenRounds = 8;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), SpawnPoints);
	
    CombatManager->MaxAttackingActors = 3;
}

void AFighterGameMode_Arena::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

    PlayerDeathTimerHandle.Invalidate();
    RoundTimerHandle.Invalidate();

    Super::EndPlay(EndPlayReason);
}

void AFighterGameMode_Arena::StartRound()
{
    GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, ("NEW ROUND"));

    RoundCount++;
    SpawnEnemies(RoundCount);
}

void AFighterGameMode_Arena::EnemyDrop(AActor* Actor)
{
    // override enemy drop to disable enemy drop in Arena Mode
}

void AFighterGameMode_Arena::PlayerDeathEvent()
{
    GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, ("LOAD HUD LEVEL"));

    UGameInstance* GameInst = UGameplayStatics::GetGameInstance(GetWorld());
    UFighterGameInstance* FighterGameInstance = Cast<UFighterGameInstance>(GameInst);
    FighterGameInstance->ExtiChallengeLevel("L_HUB", PlayerActor, LevelToTransitionTo);
}

void AFighterGameMode_Arena::StartDeathTimer(AActor* Actor)
{
    GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, ("START DEATH TIMER"));

    GetWorldTimerManager().SetTimer(PlayerDeathTimerHandle, this, &AFighterGameMode_Arena::PlayerDeathEvent, 4.f, false);
}

void AFighterGameMode_Arena::SpawnEnemies(int32 EnemyCount)
{
    if (!ActorToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyClass is not set."));
        return;
    }

    if (SpawnPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No spawn points found."));
        return;
    }

    int32 SpawnPointCount = SpawnPoints.Num();

    if (SpawnPointCount == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No spawn points found or EnemyClass not set."));
        return;
    }

    for (int32 i = 0; i < EnemyCount; ++i)
    {
        int32 SpawnIndex = i % SpawnPointCount;
        AActor* SpawnPointActor = SpawnPoints[SpawnIndex];

        if (!SpawnPointActor)
        {
            UE_LOG(LogTemp, Error, TEXT("SpawnPointActor at index %d is null."), SpawnIndex);
            continue;
        }

        FVector SpawnLocation = SpawnPointActor->GetActorLocation();
        FRotator SpawnRotation = SpawnPointActor->GetActorRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


        AEnemyBase* NewEnemy = GetWorld()->SpawnActor<AEnemyBase>(ActorToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
        
        if (NewEnemy->AIC)
        {
            NewEnemy->AIC->AttackTarget = PlayerActor;
            NewEnemy->AIC->bReachPlayer = true;
           // NewEnemy->AIC->SetStateAsAttacking();
        }
        if (!NewEnemy)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn enemy."));
        }
        else
        {
            SpawnedActors.Add(NewEnemy);
            NewEnemy->DamagableComponent->OnDeath.AddDynamic(this, &AFighterGameMode_Arena::OnEnemyDeath);
        }
    }
}

void AFighterGameMode_Arena::OnEnemyDeath(AActor* Actor)
{
    if (SpawnedActors.Num() > 0)
    {
        SpawnedActors.RemoveAt(SpawnedActors.Num() - 1);
        if (SpawnedActors.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("ROUND ENDED"));
            GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, ("ROUND FINISHED!"));
            RoundEndTimer();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnedActors is already empty when trying to remove an element"));
    }
   
}

void AFighterGameMode_Arena::RoundEndTimer()
{
    int32 RoundPrize = RoundCount * 125;
    AddMoney(RoundPrize);

    TWeakObjectPtr<AFighterGameMode_Arena> WeakThis(this);
    GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, [WeakThis]()
        {
            if (WeakThis.IsValid())
            {
                WeakThis->StartRound();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("AEnemyBase: TIMER WEAK POINTER NULL"));
            }
        }, TimeBetweenRounds, false);
}


