// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterGameMode.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Component_ResourcesDrop.h"
#include "FighterGameInstance.h"
#include "Interface_Enemy.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "EnemyBase.h"

AFighterGameMode::AFighterGameMode()
{

    PrimaryActorTick.bCanEverTick = true;

}

void AFighterGameMode::BeginPlay()
{
	Super::BeginPlay();

    GameInstance = Cast<UFighterGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	CombatManager = GetWorld()->SpawnActor<ACombatManager>(ACombatManager::StaticClass());
    CombatManager->MaxAttackingActors = MaxAttackingActors;

    PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerActor)
    {
        if (PlayerActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
        {
            UComponent_Damagable* PlayerDamagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(PlayerActor);
            PlayerDamagable->OnDeath.AddDynamic(this, &AFighterGameMode::StartDeathTimer);
            AFightingProjectCharacter* PlayerClass = Cast<AFightingProjectCharacter>(PlayerActor);
            PlayerClass->OnGamePause.AddDynamic(this, &AFighterGameMode::UnpauseGame);
        }
    }

    MoneyDropMultiplier = 1.f;
}

void AFighterGameMode::PauseGame_Implementation()
{
    //GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, ("PAUSE GAME"));
    
}

void AFighterGameMode::UnpauseGame()
{
    PauseGame();
}

void AFighterGameMode::TrySpawnItem(FVector SpawnLocation)
{
    //GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, "TRY SPAWN ITEM");

    float TotalProbability = NothingSpawnProbability;
    for (const TPair<TSubclassOf<AItem>, float>& Pair : SpawnableItem)
    {
        TotalProbability += Pair.Value;
    }

    float RandomValue = UKismetMathLibrary::RandomFloatInRange(0.0f, TotalProbability);

    float AccumulatedProbability = NothingSpawnProbability;

    if (RandomValue <= AccumulatedProbability)
    {
        UE_LOG(LogTemp, Log, TEXT("Nothing spawned"));
        return;
    }

    /*
    FVector Start = SpawnLocation;
    FVector End = SpawnLocation - FVector(0.0f, 0.0f, 10000.0f);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // Ignore the game mode actor

    UWorld* World = GetWorld();
    if (World && World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
    {
        float DistanceToGround = (SpawnLocation.Z - HitResult.ImpactPoint.Z);
        float DesiredHeightAboveGround = 50.0f; // Change this to your desired height above the ground

        if (DistanceToGround > DesiredHeightAboveGround)
        {
            SpawnLocation.Z = HitResult.ImpactPoint.Z + DesiredHeightAboveGround;
        }
    }*/

    for (const TPair<TSubclassOf<AItem>, float>& Pair : SpawnableItem)
    {
        AccumulatedProbability += Pair.Value;
        if (RandomValue <= AccumulatedProbability)
        {
            UWorld* W = GetWorld();
            if (W)
            {
                FActorSpawnParameters SpawnParams;
                W->SpawnActor<AItem>(Pair.Key, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
                UE_LOG(LogTemp, Log, TEXT("Spawned: %s"), *Pair.Key->GetName());
            }
            return;
        }
    }
}

void AFighterGameMode::SpawnItem(FVector SpawnLocation, TSubclassOf<AItem> ItemToSpawn)
{
    UWorld* W = GetWorld();
    if (W)
    {
        FActorSpawnParameters SpawnParams;
        W->SpawnActor<AItem>(ItemToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        UE_LOG(LogTemp, Log, TEXT("Spawned: %s"), *ItemToSpawn->GetName());
    }
}

void AFighterGameMode::EnemyDrop(AActor* Actor)
{
    if (Actor == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "ENEMY DROP: ACTOR NULL");

        return;
    }

    AEnemyBase* Enemy = Cast<AEnemyBase>(Actor);

    int32 MoneyToDrop = Enemy->ResourcesComponent->Money;

    float Offset = 0.2f * MoneyToDrop;

    float MinValue = MoneyToDrop - Offset;
    float MaxValue = MoneyToDrop + Offset;


    int32 RandomVariance = UKismetMathLibrary::RandomIntegerInRange(static_cast<int32>(MinValue), static_cast<int32>(MaxValue));
    MoneyToDrop = RandomVariance * MoneyDropMultiplier;

    AddMoney(MoneyToDrop);

}

void AFighterGameMode::AddMoney(int32 Amount)
{
    GameInstance->MoneyAmount += Amount;
   // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("My Int Value: %d"), GameInstance->MoneyAmount));
    OnMoneyDrop.Broadcast(Amount, false);
}

void AFighterGameMode::SubtractMoney(int32 Amount)
{
    GameInstance->MoneyAmount -= Amount;
    // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("My Int Value: %d"), GameInstance->MoneyAmount));
    OnMoneyDrop.Broadcast(Amount, true);
}



void AFighterGameMode::PlayerDeathEvent()
{
   // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Orange, "DEATH EVENT");

    UGameInstance* GameInst = UGameplayStatics::GetGameInstance(GetWorld());
    UFighterGameInstance* FighterGameInstance = Cast<UFighterGameInstance>(GameInst);
    FighterGameInstance->GameOver(GameOverLevelToLoad);
}

void AFighterGameMode::StartDeathTimer(AActor* Actor)
{
  //  GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Orange, "DEATH START TIMER");

    GetWorldTimerManager().SetTimer(PlayerDeathTimerHandle, this, &AFighterGameMode::PlayerDeathEvent, 2.f, false);
}

void AFighterGameMode::ClearAllTimers()
{
    TArray<AActor*> FoundActors;
    TSubclassOf<AEnemyBase> ActorClass = AEnemyBase::StaticClass();;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorClass, FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (Actor)
        {
            FString OwnerName = Actor ? *Actor->GetName() : TEXT("No Owner");
            //GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, FString::Printf(TEXT("DESTROY ENEMY Actor: %s"), *OwnerName));

            AEnemyBase* enemy = IInterface_Enemy::Execute_GetOwnerClass(Actor);
            enemy->InvalidateAllTimers();

            Actor->Destroy();
        }
    }

    GetWorldTimerManager().ClearAllTimersForObject(PlayerActor);
}

AFighterGameMode* AFighterGameMode::GetFighterGameMode_Implementation()
{
    return this;
}

